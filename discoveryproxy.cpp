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
//#include "qdom.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QDomNode>

const char* xmlns_dlna = "xmlns:dlna";
const char* schema_device = "urn:schemas-dlna-org:device-1-0";
const char* service_type = "upnp:urn:schemas-upnp-org:service:RemoteUIServer:1";
const char* service_urn = "urn:upnp-org:serviceId:RemoteUIServer";

DiscoveryProxy::DiscoveryProxy()
    : m_soapHttp(this), m_http(this)
{
    // Connect signals to slots.
    connect(&m_http, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReply(QNetworkReply*)));
    connect(&m_soapHttp, SIGNAL(finished(QNetworkReply*)), this, SLOT(soapHttpReply(QNetworkReply*)));

#ifdef DISCOVERY_STUB
    UPnPDeviceList serverList = DiscoveryStub::Instance()->startServiceDiscovery(service_type, this );
    //processServerList(serverList);
#else
// Start real discovery
#endif
}

// Here with an updated server list from the Disovery module (callback)
void DiscoveryProxy::serverListUpdate(std::string type, UPnPDeviceList deviceList)
{
    if ( type.compare(service_type) != 0 ) {
        fprintf(stderr,"\nServer List Update: Invalid service type: %s\n", type.c_str());
    } else {
        processDeviceList(deviceList);
    }
}

// Here to request a device description for each server
void DiscoveryProxy::processDeviceList(UPnPDeviceList deviceList)
{
    m_deviceList = deviceList;

    UPnPDeviceList::iterator p;

    fprintf(stderr,"\nServer List Update:\n");

    for (p = m_deviceList.begin(); p!=m_deviceList.end(); ++p) {

        UPnPDevice device = p->second;
        fprintf(stderr,"RUI Server: %s -> %s\n", device.friendlyName.c_str(), device.descURL.c_str());

        // Request RUI Server Description
        QNetworkRequest networkReq;
        networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
        networkReq.setUrl(QUrl(device.descURL.c_str()));

        m_http.get(networkReq);
    }
}

void DiscoveryProxy::processDevice(const QString& url, const QDomDocument& document)
{
    QString baseURL = url.left(url.lastIndexOf("/"));

    // A device can have multiple devices and each device can have multiple services.
    // Filter by the service we are interested in.

    QDomNodeList nodes = document.elementsByTagName("service");

    for (int i=0; i < nodes.count(); i++) {

        QDomNode node = nodes.item(i);
        QDomElement elem = node.firstChildElement("serviceId");
        QString serviceId = elem.text();
        if (serviceId.compare(service_urn) == 0) {
            elem = node.firstChildElement("controlURL");
            QString controlURL = baseURL + elem.text();
            fprintf( stderr, "controlURL: %s\n", controlURL.toAscii().data());
            requestCompatibleUIs(controlURL);
        }
    }
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

    fprintf( stderr, "soap message:\n%s\n", xml.toAscii().data());

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
    fprintf( stderr, "http reply from url: %s\n%s\n", url.toAscii().data(),xml.toAscii().data());

    // TODO: Need to trim \r\n and spaces from element text!

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

    QString xml(reply->readAll());

    QString url = reply->url().toString();
    fprintf( stderr, "DiscoveryProxy::soapHttpReply from url: %s\n%s\n", url.toAscii().data(),xml.toAscii().data());

    // Parse the reply, create document
    QString errorMessage;
    int errorLine,errorColumn;

    QDomDocument document;
    if (!document.setContent(xml, &errorMessage, &errorLine, &errorColumn)) {
        fprintf(stderr,"setContent failed. Line: %d, Column: %d, Error: %s\n",
                errorLine, errorColumn, errorMessage.toAscii().data() );
        return;
    }

    /*
  <ui>
    <uiID>UI_ID</uiID>
    <name>UI_NAME</name>
    <description>UI_DESC</description>
    <iconList>
      <icon>
        <mimetype>image/png</mimetype>
        <width>UI_IMG_WIDTH</width>
        <height>UI_IMAGE_HEIGHT</height>
        <depth>0</depth>
        <url>/UI_BASE.png</url>
      </icon>
    </iconList>
    <fork>0</fork>
    <lifetime>1</lifetime>
    <protocol shortName="HTTP">
      <uri>DLNA-HTML5-1.0://localhostip:port/UI_BASE.html</uri>
      <protocolInfo>http</protocolInfo>
    </protocol>
  </ui>     */


}

