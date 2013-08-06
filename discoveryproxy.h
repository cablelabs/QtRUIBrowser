/*
 * Copyright (C) 2012, 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef DISCOVERYPROXY_H
#define DISCOVERYPROXY_H

// FIXME: We really shouldn't be including from WebKit's private API.
#include "config.h"
#include "DiscoveryWrapper.h"

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

    void processDeviceList(UPnPDevMap);
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
    virtual void serverListUpdate(String type, UPnPDevMap *devs);

    // IDiscoveryAPI - not implemented
    virtual void UPnPDevAdded(String, UPnPDevice&) {}
    virtual void UPnPDevDropped(String, UPnPDevice&) {}
    virtual void ZCDevAdded(String, ZCDevice&) {}
    virtual void ZCDevDropped(String, ZCDevice&) {}
    virtual void sendEvent(String, String, String) {}
    virtual void onError(int) {}
    virtual void onZCError(int) {}
    virtual void receiveID(long) {}

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

