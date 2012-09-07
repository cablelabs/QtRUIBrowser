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

class DiscoveryProxy : public QObject, public IDiscoveryAPI
{
    Q_OBJECT
public:
    DiscoveryProxy();

private:

    void dumpServerListToConsole(UPnPDeviceList list);

public slots:
    // Public JavaScript API (bridge)

private slots:
    // IDiscoveryAPI
    virtual void serverListUpdate(std::string type, UPnPDeviceList devs);

};

#endif // DISCOVERYPROXY_H

