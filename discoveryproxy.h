/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * discoveryproxy.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Proxy for the WebKit Discovery module and RUI list management
 *
 */

#ifndef DISCOVERYPROXY_H
#define DISCOVERYPROXY_H

#define DISCOVERY_STUB

#ifdef DISCOVERY_STUB
#include "discoverystub.h"
#else
// Gar's files go here.
#endif

#include <QObject>
#include <QDomDocument>
#include <QNetworkAccessManager>

class DiscoveryProxy : public QObject, public IDiscoveryAPI
{
    Q_OBJECT

public:
    DiscoveryProxy();

private:

    UPnPDeviceList m_deviceList;

    QNetworkAccessManager m_soapHttp;
    QNetworkAccessManager m_http;

    void processDeviceList(UPnPDeviceList);
    void processDevice(const QString& url, const QDomDocument&);
    void requestCompatibleUIs(const QString&);

public slots:

    // Public JavaScript API (bridge)

private slots:

    // IDiscoveryAPI
    virtual void serverListUpdate(std::string type, UPnPDeviceList devs);

    // HTTP
    void httpReply(QNetworkReply*);
    void soapHttpReply(QNetworkReply*);
};

#endif // DISCOVERYPROXY_H

