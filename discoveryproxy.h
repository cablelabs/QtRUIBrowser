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

//#include "NavDsc.h"
//#include "IDiscoveryAPI.h"
#include "DiscoveryWrapper.h"

typedef std::map<std::basic_string<char>, UPnPDevice> UPnPDeviceList;

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
    static DiscoveryProxy* Instance();

    bool isHostRUITransportServer(const QString& hostURL);

    // Debugging
    void dumpUserInterfaceMap();
    bool m_home;

private:

    DiscoveryProxy();
    static DiscoveryProxy* m_pInstance;
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
    QString userAgentString();
signals:

    void ruiListNotification();
    void ruiDeviceAvailable(QString);

public slots:

    // Public JavaScript API (bridge)
    QVariantList ruiList();
    void console(const QString&);
    int scrollIndex();
    int screenIndex();
    void setScrollIndex(int index);
    void setScreenIndex(int index);

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
    virtual void receiveID(long idFromHN) {}


    // For executing on main thread.
    void requestDeviceDescription(QString);

    // HTTP
    void httpReply(QNetworkReply*);
    void soapHttpReply(QNetworkReply*);

public:
    // JavaScript state variables
    // These public variables are used to persist state on the navigation page, so when you return
    // from a rui page, the last selected row and scroll position are restored.
    int m_scrollIndex;
    int m_screenIndex;

};

#endif // DISCOVERYPROXY_H

