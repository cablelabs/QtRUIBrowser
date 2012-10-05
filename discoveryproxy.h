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

#include "NavDsc.h"
#include "IDiscoveryAPI.h"

typedef std::map<std::basic_string<char>, sUPnPDevice> UPnPDeviceList;

using namespace WebCore;

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QDomDocument>
#include <QNetworkAccessManager>

#include "userinterfacemap.h"
Q_DECLARE_METATYPE(UPnPDevice)

class DiscoveryProxy : public QObject, public IDiscoveryAPI
{
    Q_OBJECT

public:
    DiscoveryProxy();

    bool isHostRUIHServer(const QString& hostURL);

    // Debugging
    void dumpUserInterfaceMap();
    bool m_home;

private:

    UserInterfaceMap m_userInterfaceMap;
    QNetworkAccessManager m_soapHttp;
    QNetworkAccessManager m_http;

    void processDeviceList(UPnPDeviceList);
    void processDevice(const QString& url, const QDomDocument& document);
    void processUIList(const QString& url, const QDomDocument& document);
    void notifyListChanged();
    void requestCompatibleUIs(const QString&);
    QString trimElementText(const QString&);
    QString elementTextForTag(const QDomNode& parent, const QString& tag);

signals:

    void ruiListNotification();
    void ruiDeviceAvailable(QString);

public slots:

    // Public JavaScript API (bridge)
    QVariantList ruiList();
    void console(const QString&);

private slots:

    // IDiscoveryAPI
    virtual void serverListUpdate(std::string type, UPnPDeviceList *devs);

    // IDiscoveryAPI - not implemented
    virtual void UPnPDevAdded(std::string) {}
    virtual void UPnPDevDropped(std::string) {}
    virtual void ZCDevAdded(std::string) {}
    virtual void ZCDevDropped(std::string) {}
    virtual void sendEvent(std::string, std::string, std::string) {}
    virtual void onError(int) {}

    // For executing on main thread.
    void requestDeviceDescription(QString);

    // HTTP
    void httpReply(QNetworkReply*);
    void soapHttpReply(QNetworkReply*);
};

#endif // DISCOVERYPROXY_H

