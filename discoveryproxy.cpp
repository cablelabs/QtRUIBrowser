/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * discoveryproxy.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Proxy for the WebKit Discovery module and RUI list management
 *
 */

#include "discoveryproxy.h"
#include "soapmessage.h"

#include <stdio.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QDomNode>
#include <QDomDocumentFragment>
#include <QMapNode>
#include <QMap>
#include <QVariant>
#include <QVariantMap>
#include <QTextDocument>
//#include <QUrl>
#include "ruiwebpage.h"

DiscoveryProxy* DiscoveryProxy::m_pInstance = NULL;


const char* xmlns_dlna = "xmlns:dlna";
const char* schema_device = "urn:schemas-dlna-org:device-1-0";
const char* service_type = "urn:schemas-upnp-org:service:RemoteUIServer:1";
const char* service_urn = "urn:upnp-org:serviceId:RemoteUIServer";

DiscoveryProxy::DiscoveryProxy()
    : m_home(false), m_soapHttp(this), m_http(this)
{
    // Connect signals to slots.
    connect(&m_http, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReply(QNetworkReply*)));
    connect(&m_soapHttp, SIGNAL(finished(QNetworkReply*)), this, SLOT(soapHttpReply(QNetworkReply*)));
    connect(this, SIGNAL(ruiDeviceAvailable(QString)), this, SLOT(requestDeviceDescription(QString)));

    // Start discovery
    UPnPDeviceList deviceList = NavDsc::getInstance()->startUPnPInternalDiscovery(service_type, this );
    processDeviceList(deviceList);

    // JavaScript state variables
    m_scrollIndex = 0;
    m_screenIndex = 0;
}

int DiscoveryProxy::scrollIndex()
{
    return m_scrollIndex;
}

int DiscoveryProxy::screenIndex()
{
    return m_screenIndex;
}

void DiscoveryProxy::setScrollIndex(int index)
{
    m_scrollIndex = index;
}

void DiscoveryProxy::setScreenIndex(int index)
{
    m_screenIndex = index;
}


// Here with an updated server list from the Disovery module (callback)
void DiscoveryProxy::serverListUpdate(std::string type, UPnPDeviceList *deviceList)
{
    if ( type.compare(service_type) != 0 ) {
        fprintf(stderr,"\nServer List Update: Invalid service type: %s\n", type.c_str());
    } else {
        processDeviceList(*deviceList);
    }
}

// Here on a SLOT to execute http request on main thread (signaled from serverListUpdate()
void DiscoveryProxy::requestDeviceDescription( QString url)
{
    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setUrl(url);

    m_http.get(networkReq);
}


// Here to request a device description for each server
void DiscoveryProxy::processDeviceList(UPnPDeviceList deviceList)
{
    UPnPDeviceList::iterator p;

    //fprintf(stderr,"\nServer List Update:\n");

    QStringList devices;

    // Request device descriptions
    for (p = deviceList.begin(); p!=deviceList.end(); ++p) {

        UPnPDevice device = p->second;
        //fprintf(stderr," - Server: %s -> %s [%s]\n", device.friendlyName.c_str(), device.descURL.c_str(), device.uuid.c_str());

        // Process device on main thread.
        emit ruiDeviceAvailable(QString(device.descURL.c_str()));

        devices.append(QString(device.uuid.c_str()));
    }

    // Check for deletions
    int deleteCount = m_userInterfaceMap.checkForRemovedDevices(devices);

    if (deleteCount > 0) {
        notifyListChanged();
    }
}

void DiscoveryProxy::notifyListChanged()
{
    if (m_home) {
        //fprintf( stderr, "Notify JavaScript (rui list changed)\n");
        emit ruiListNotification();
    }
}

// TODO: I temporarily disabled support for absolute and slash(/) prefixed uris as it is breaking
// the CES demo.

// Here with a new root device description. Process the root device and any nested devices.
void DiscoveryProxy::processDevice(const QString& url, const QDomDocument& document)
{
    QString baseURL = url.left(url.lastIndexOf("/"));

    // Optional and deprecated.
    QDomElement rootElement = document.documentElement();
    if (!rootElement.isNull()) {
        QDomElement urlBase = rootElement.firstChildElement("URLBase");
        if (!urlBase.isNull()) {
            baseURL = trimElementText(urlBase.text());
        }
    }

    // Get base URL without path for slash prefixed relative URIs
    QUrl qurl = QUrl(baseURL);
    QString hostURL = qurl.toString(QUrl::RemovePath);


    // A device can have multiple devices and each device can have multiple services.
    // Filter by the device/service pairs we are interested in. Record the uuid of the root device
    // for all devices (including the root) so we can determine which devices to delete on a removal.

    QDomNodeList deviceList = document.elementsByTagName("device");

    QString rootDeviceUuid;

    for (int i=0; i < deviceList.count(); i++) {

        QDomNode device = deviceList.item(i);

        // There can be multiple X_DLNADOC elements for a device. We don't care about the value at this point,
        // but want to ensure that this is in fact a DLNA device.
        // <dlna:X_DLNADOC>DMS-1.50</dlna:X_DLNADOC>
        QDomElement dlnaDoc = device.firstChildElement("dlna:X_DLNADOC");
        if (dlnaDoc.isNull()) {
            continue;
        }

        RUIDevice ruiDevice;

        QString uuid = trimElementText(device.firstChildElement("UDN").text());

        if (i == 0) {

            rootDeviceUuid = uuid;
        }

        ruiDevice.m_friendlyName = trimElementText(device.firstChildElement("friendlyName").text());
        ruiDevice.m_uuid = uuid;
        ruiDevice.m_rootDeviceUuid = rootDeviceUuid;
        ruiDevice.m_baseURL = baseURL;

        // We are only interested devices that implement the the RemoteUIServer service.
        QDomElement serviceList = device.firstChildElement("serviceList");
        if (!serviceList.isNull()) {

            QString tag = "service";
            QDomElement service = serviceList.firstChildElement(tag);
            while (!service.isNull()) {

                QDomElement serviceType = service.firstChildElement("serviceType");

                QString serviceTypeText = trimElementText(serviceType.text());
                if (serviceTypeText.compare(service_type) == 0) {

                    // Matching service type.
                    RUIService ruiService;
                    ruiService.m_baseURL = baseURL;

                    QDomElement urlElement;
                    QString trimmedURL;

                    // Service Control URL
                    urlElement = service.firstChildElement("controlURL");
                    if (!urlElement.isNull()) {
                        trimmedURL = trimElementText(urlElement.text());
                        if ( false && trimmedURL.contains("://")) {
                            ruiService.m_controlURL = trimmedURL;
                        }
                        else {
                            if ( false && trimmedURL[0] == '/') {
                                ruiService.m_controlURL = hostURL + trimmedURL;
                            }
                            else {
                                ruiService.m_controlURL = baseURL + trimmedURL;
                            }
                        }
                    }

                    // Service Description URL
                    urlElement = service.firstChildElement("SCPDURL");
                    if (!urlElement.isNull()) {
                        trimmedURL = trimElementText(urlElement.text());
                        if (trimmedURL.contains("://")) {
                            ruiService.m_descriptionURL = trimmedURL;
                        }
                        else {
                            if (trimmedURL[0] == '/') {
                                ruiService.m_descriptionURL = hostURL + trimmedURL;
                            }
                            else {
                                ruiService.m_descriptionURL = baseURL + trimmedURL;
                            }
                        }
                    }

                    // Service Eventing URL
                    urlElement = service.firstChildElement("eventSubURL");
                    if (!urlElement.isNull()) {
                        trimmedURL = trimElementText(urlElement.text());
                        if (trimmedURL.contains("://")) {
                            ruiService.m_eventURL = trimmedURL;
                        }
                        else {
                            if (trimmedURL[0] == '/') {
                                ruiService.m_eventURL = hostURL + trimmedURL;
                            }
                            else {
                                ruiService.m_eventURL = baseURL + trimmedURL;
                            }
                        }
                    }

                    ruiService.m_serviceType = trimElementText(serviceType.text());

                    QDomElement serviceID = service.firstChildElement("serviceId");
                    ruiService.m_serviceID = baseURL + trimElementText(serviceID.text());

                    ruiDevice.m_serviceList.append(ruiService);

                    fprintf( stderr, "Request Compatible UIs: %s\n", ruiService.m_controlURL.toAscii().data());
                    requestCompatibleUIs(ruiService.m_controlURL);
                }

                // Loop through all of the services for this device.
                service = service.nextSiblingElement(tag).toElement();
            }
        }

        if (ruiDevice.m_serviceList.count()) {

            m_userInterfaceMap.addDevice(ruiDevice);

        } else {
            if ( m_userInterfaceMap.deviceExists(ruiDevice.m_uuid)) {
                fprintf(stderr," - Removing device - no longer provides RUI service: %s - %s\n", ruiDevice.m_uuid.toAscii().data(), url.toAscii().data());
                m_userInterfaceMap.removeDevice(ruiDevice.m_uuid);
            }
        }
    }
}

void DiscoveryProxy::processUIList(const QString& url, const QDomDocument& document)
{
    QString baseURL = url.left(url.lastIndexOf("/"));

    QString serviceKey = url;
    QList<RUIInterface> serviceUIs;

    //fprintf(stderr, "Processing UI List: %s\n", url.toAscii().data());

    QDomNodeList uiList = document.elementsByTagName("ui");

    for (int i=0; i < uiList.count(); i++) {

        bool protocolMatch = false;

        QDomNode ui = uiList.item(i);
        RUIInterface ruiInterface;

        ruiInterface.m_uiID = trimElementText(ui.firstChildElement("uiID").text());
        ruiInterface.m_name = trimElementText(ui.firstChildElement("name").text());
        ruiInterface.m_description = trimElementText(ui.firstChildElement("description").text());

        // Icons
        QDomElement iconList = ui.firstChildElement("iconList");
        if (!iconList.isNull()) {

            QString tag = "icon";
            QDomElement icon = iconList.firstChildElement(tag);
            while (!icon.isNull()) {

                RUIIcon ruiIcon;

                ruiIcon.m_mimeType = elementTextForTag(icon, "mimetype");
                ruiIcon.m_url = baseURL + elementTextForTag(icon, "url");
                ruiIcon.m_width = elementTextForTag(icon, "width");
                ruiIcon.m_height = elementTextForTag(icon, "height");
                ruiIcon.m_depth = elementTextForTag(icon, "depth");

                ruiInterface.m_iconList.append(ruiIcon);

                // Loop through all of the icons for this device.
                icon = icon.nextSiblingElement(tag).toElement();
            }

        }

        if (ruiInterface.m_iconList.count() == 0) {
            RUIIcon missingIcon;

            missingIcon.m_mimeType = "image/png";
            missingIcon.m_url = "qrc:/www/rui_missingIcon.png";
            missingIcon.m_width = "40";
            missingIcon.m_height = "40";
            missingIcon.m_depth = "24";

            ruiInterface.m_iconList.append(missingIcon);
        }

        // Protocols
        QString protocolTag = "protocol";
        QDomElement protocol = ui.firstChildElement(protocolTag);
        while (!protocol.isNull()) {

            RUIProtocol ruiProtocol;

            ruiProtocol.m_shortName = protocol.attribute("shortName");

            if ( ruiProtocol.m_shortName.compare("DLNA-HTML5-1.0") == 0) {

                protocolMatch = true;
                ruiProtocol.m_protocolInfo = elementTextForTag(protocol, "protocolInfo");

                QString uriTag = "uri";

                QDomElement uri = protocol.firstChildElement(uriTag);
                while (!uri.isNull()) {
                    ruiProtocol.m_uriList.append(trimElementText(uri.text()));
                    uri = uri.nextSiblingElement(uriTag).toElement();
                }

                ruiInterface.m_protocolList.append(ruiProtocol);
            }


            // Loop through all of the icons for this device.
            protocol = protocol.nextSiblingElement(protocolTag).toElement();
        }

        if (protocolMatch) {
            serviceUIs.append(ruiInterface);
        }
    }

    m_userInterfaceMap.addServiceUIs(serviceKey, serviceUIs);

    notifyListChanged();
}

QString DiscoveryProxy::userAgentString() {
    // OK, this is horky. But the only way to get the standard user agent string is through a RUIWebPage.
    RUIWebPage tempPage;
    return tempPage.userAgentForUrl(QUrl());
}


// Here with a qualified controlURL for a RemoteUIServer service.
void DiscoveryProxy::requestCompatibleUIs(const QString& url)
{
    // Build our soap message
    SoapMessage soapMessage;
    soapMessage.setMethod("u:GetCompatibleUIs", "xmlns:u", "urn:schemas-upnp-org:service:RemoteUIServer:1");
    soapMessage.addMethodArgument("InputDeviceProfile","");
    soapMessage.addMethodArgument("UIFilter","*");

    QString xml = soapMessage.message();

    //fprintf( stderr, "soap message:\n%s\n", xml.toAscii().data());

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", "GetCompatibleUIs");
    networkReq.setRawHeader("User-Agent", userAgentString().toAscii().data());
    networkReq.setUrl(QUrl(url));

    QNetworkReply* reply = m_soapHttp.post(networkReq, xml.toAscii());
    reply->setReadBufferSize(1024*250); // 7.3.2.15.2
    //fprintf(stderr,"Read buffer size: %d\n", (int) reply->readBufferSize());
}

// We have received a RUI Server Description. Parse the control URL and request compatible UIs.
void DiscoveryProxy::httpReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString httpStatusMessage = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
        fprintf( stderr, "DiscoveryProxy::httpReply: error %d -  %s\n", httpStatus, httpStatusMessage.toAscii().data() );
        return;
    }

    QString xml(reply->readAll());
    QString url = reply->url().toString();


    //fprintf( stderr, "http reply (RUI Server Description) from url: %s\n%s\n", url.toAscii().data(),xml.toAscii().data());

    // Parse the reply, create document
    QString errorMessage;
    int errorLine,errorColumn;

    QDomDocument document;
    if (!document.setContent(xml, &errorMessage, &errorLine, &errorColumn)) {
        fprintf(stderr,"setContent failed. Line: %d, Column: %d, Error: %s\n",
                errorLine, errorColumn, errorMessage.toAscii().data() );
        return;
    }

    processDevice(url, document);
}

// We have received a list of compatible UIs
void DiscoveryProxy::soapHttpReply(QNetworkReply* reply)
{
    int errorCode = reply->error();
    QString errorString = reply->errorString();
    if (errorCode != QNetworkReply::NoError) {
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString httpStatusMessage = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
        fprintf( stderr, "DiscoveryProxy::soapHttpReply: error %d - %s \n   - (http status %d -  %s)\n   - Request URI: %s\n",
                 errorCode, errorString.toAscii().data(),
                 httpStatus, httpStatusMessage.toAscii().data(),
                 reply->url().toString().toAscii().data()
                 );
        return;
    }

    QString html(reply->readAll());
    QTextDocument text;
    text.setHtml(html);
    QString xml = text.toPlainText();
    //QString xml = html;
    //fprintf( stderr, "Received reply: %d bytes\n", html.length());


    QString url = reply->url().toString();
    //fprintf( stderr, "DiscoveryProxy::soapHttpReply from url: %s\n%s\n", url.toAscii().data(),xml.toAscii().data());

    // Parse the reply, create document
    QString errorMessage;
    int errorLine,errorColumn;

    QDomDocument document;
    if (!document.setContent(xml, &errorMessage, &errorLine, &errorColumn)) {
        fprintf(stderr,"setContent failed. Line: %d, Column: %d, Error: %s\n",
                errorLine, errorColumn, errorMessage.toAscii().data() );
        return;
    }

    processUIList(url, document);
}

// Here to return the trimmed text of a single occurance child element.
QString DiscoveryProxy::elementTextForTag(const QDomNode& parent, const QString& tag)
{
    QDomElement element = parent.firstChildElement(tag);
    return trimElementText(element.text());
}


QString DiscoveryProxy::trimElementText(const QString& str)
{
    QString temp = "";

    // Trim end
    int n = str.size() - 1;
    for (; n >= 0; --n) {

        char c = str.at(n).toAscii();
        if (!(c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
            temp = str.left(n+1);
            break;
        }
    }

    // Trim beginning
    for (n=0; n < temp.size(); n++) {

        char c = temp.at(n).toAscii();
        if (!(c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
            temp = temp.right(temp.size()-n);
            break;
        }
    }

    return temp;
}

void DiscoveryProxy::dumpUserInterfaceMap()
{
    m_userInterfaceMap.dumpToConsole();
}

// Here to return a list of RUIs to javascript
QVariantList DiscoveryProxy::ruiList()
{
    return m_userInterfaceMap.generateUIList();
}

// JavaScript output to application console.
void DiscoveryProxy::console(const QString& str)
{
    fprintf( stderr,"%s\n", str.toAscii().data());
}

DiscoveryProxy* DiscoveryProxy::Instance()
{
    if ( !m_pInstance )
    {
        m_pInstance = new DiscoveryProxy;
    }

    return m_pInstance;
}

bool DiscoveryProxy::isHostRUITransportServer(const QString& hostURL)
{
    return m_userInterfaceMap.isHostRUITransportServer(hostURL);
}


