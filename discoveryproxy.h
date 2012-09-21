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
#include <QVariant>
#include <QVariantMap>
#include <QDomDocument>
#include <QNetworkAccessManager>

#include "userinterfacemap.h"

class DiscoveryProxy : public QObject, public IDiscoveryAPI
{
    Q_OBJECT

public:
    DiscoveryProxy();

    bool isHostRUIHServer(const QString& hostURL);

    // Debugging
    void dumpUserInterfaceMap();

private:

    UserInterfaceMap m_userInterfaceMap;
    QNetworkAccessManager m_soapHttp;
    QNetworkAccessManager m_http;

    void processDeviceList(UPnPDeviceList);
    void processDevice(const QString& url, const QDomDocument& document);
    void processUIList(const QString& url, const QDomDocument& document);
    void requestCompatibleUIs(const QString&);
    QString trimElementText(const QString&);
    QString elementTextForTag(const QDomNode& parent, const QString& tag);

signals:

    void ruiListNotification();

public slots:

    // Public JavaScript API (bridge)
    //QVariantMap ruiList();
    QVariantList ruiList();
    void console(const QString&);

private slots:

    // IDiscoveryAPI
    virtual void serverListUpdate(std::string type, UPnPDeviceList devs);

    // HTTP
    void httpReply(QNetworkReply*);
    void soapHttpReply(QNetworkReply*);
};

#endif // DISCOVERYPROXY_H

