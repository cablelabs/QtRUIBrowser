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

    fprintf(stderr,"\nServer List Update:\n");

    QStringList devices;

    // Request device descriptions
    for (p = deviceList.begin(); p!=deviceList.end(); ++p) {

        UPnPDevice device = p->second;
        fprintf(stderr," - Server: %s -> %s [%s]\n", device.friendlyName.c_str(), device.descURL.c_str(), device.uuid.c_str());

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
        fprintf( stderr, "Notify JavaScript (rui list changed)\n");
        emit ruiListNotification();
    }
}

// Here with a new root device description. Process the root device and any nested devices.
void DiscoveryProxy::processDevice(const QString& url, const QDomDocument& document)
{
    QString baseURL = url.left(url.lastIndexOf("/"));

    // A device can have multiple devices and each device can have multiple services.
    // Filter by the device/service pairs we are interested in. Record the uuid of the root device
    // for all devices (including the root) so we can determine which devices to delete on a removal.

    QDomNodeList deviceList = document.elementsByTagName("device");

    QString rootDeviceUuid;

    for (int i=0; i < deviceList.count(); i++) {

        QDomNode device = deviceList.item(i);

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

                    QDomElement controlURL = service.firstChildElement("controlURL");
                    ruiService.m_controlURL = baseURL + trimElementText(controlURL.text());

                    QDomElement descriptionURL = service.firstChildElement("SCPDURL");
                    ruiService.m_descriptionURL = baseURL + trimElementText(descriptionURL.text());

                    QDomElement eventURL = service.firstChildElement("eventSubURL");
                    ruiService.m_eventURL = baseURL + trimElementText(eventURL.text());

                    ruiService.m_serviceType = trimElementText(serviceType.text());

                    QDomElement serviceID = service.firstChildElement("serviceId");
                    ruiService.m_serviceID = baseURL + trimElementText(serviceID.text());

                    ruiDevice.m_serviceList.append(ruiService);

                    //fprintf( stderr, "Request Compatible UIs: %s\n", ruiService.m_controlURL.toAscii().data());
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
    networkReq.setUrl(QUrl(url));

    m_soapHttp.post(networkReq, xml.toAscii());
}

// We have received a RUI Server Description. Parse the control URL and request compatible UIs.
void DiscoveryProxy::httpReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        fprintf( stderr, "http reply: error %d\n", reply->error());
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

    // We currently only receive device messages, but allow for others in the future.
    QDomElement root = document.documentElement();
    QString schema = root.attribute(xmlns_dlna);
    if (schema.compare(schema_device) == 0) {
        processDevice(url, document);
    }
}

// We have received a list of compatible UIs
void DiscoveryProxy::soapHttpReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        fprintf( stderr, "DiscoveryProxy::soapHttpReply: error %d\n", reply->error());
        return;
    }

    QString html(reply->readAll());
    QTextDocument text;
    text.setHtml(html);
    QString xml = text.toPlainText();
    //QString xml = html;


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


