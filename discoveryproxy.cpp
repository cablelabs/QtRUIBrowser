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
#include "qdom.h"

const std::string rui_type = "upnp:urn:schemas-upnp-org:service:RemoteUIServer:1";

DiscoveryProxy::DiscoveryProxy()
    : m_http(this), m_soapHttp(this)
{
    // Connect signals to slots.
    connect(&m_http, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReply(QNetworkReply*)));
    connect(&m_soapHttp, SIGNAL(finished(QNetworkReply*)), this, SLOT(soapHttpReply(QNetworkReply*)));

    // Build our soap message
    m_soapMessageGetCompatibleUIs.setMethod("u:GetCompatibleUIs", "xmlns:u", "urn:schemas-upnp-org:service:RemoteUIServer:1");
    m_soapMessageGetCompatibleUIs.addMethodArgument("InputDeviceProfile","");
    m_soapMessageGetCompatibleUIs.addMethodArgument("UIFilter","*");

    QString xml = m_soapMessageGetCompatibleUIs.message();
    fprintf( stderr, "soapEnvelope:\n%s\n", xml.toAscii().data());

#ifdef DISCOVERY_STUB
    UPnPDeviceList serverList = DiscoveryStub::Instance()->startServiceDiscovery(rui_type, this );
    //processServerList(serverList);
#else
// Start real discovery
#endif
}

// Here with an updated server list from the Disovery module (callback)
void DiscoveryProxy::serverListUpdate(std::string type, UPnPDeviceList serverList)
{
    if ( type.compare(rui_type) != 0 ) {
        fprintf(stderr,"\nServer List Update: Invalid service type: %s\n", type.c_str());
    } else {
        processServerList(serverList);
    }
}

// Here to request a device description for each server
void DiscoveryProxy::processServerList(UPnPDeviceList serverList)
{
    m_serverList = serverList;

    UPnPDeviceList::iterator p;

    fprintf(stderr,"\nServer List Update:\n");

    for(p = serverList.begin(); p!=serverList.end(); ++p) {

        UPnPDevice device = p->second;
        fprintf(stderr,"RUI Server: %s -> %s\n", device.friendlyName.c_str(), device.host.c_str());

        // Request RUI Server Description
        //m_http.get(QNetworkRequest(QUrl(device.descURL)));
    }
}
/*
void DiscoveryProxy::requestCompatibleUIs(const QString& url)
{
    /*
    m_soapHttp.setAction("urn:schemas-upnp-org:service:RemoteUIServer:1#GetCompatibleUIs");

    QtSoapMessage request;
    request.setMethod(QtSoapQName("GetCompatibleUIs", "urn:schemas-upnp-org:service:RemoteUIServer:1"));
    request.addMethodArgument("InputDeviceProfile", "", "");
    request.addMethodArgument("UIFilter", "", "*");

    m_soapHttp.submitRequest(request, url);
    *//*

    QNetworkAccessManager manager;
    QString data = document.toString();

    QNetworkRequest networkReq;
    networkReq.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("text/xml;charset=utf-8"));
    networkReq.setRawHeader("SOAPAction", "myurl");
    networkReq.setUrl(url);

    QNetworkReply *r = manager.post(networkReq, data.toAscii());
}*/

// We have received a RUI Server Description
void DiscoveryProxy::httpReply(QNetworkReply* reply)
{
    QDomDocument doc();
    //doc.setContent(message);
}

// We have received a list of compatible UIs
void DiscoveryProxy::soapHttpReply(QNetworkReply* reply)
{

}

