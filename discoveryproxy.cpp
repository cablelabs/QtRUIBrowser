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
#include <stdio.h>

const std::string rui_type = "upnp:urn:schemas-upnp-org:service:RemoteUIServer:1";

DiscoveryProxy::DiscoveryProxy()
{


#ifdef DISCOVERY_STUB
    UPnPDeviceList serverList = DiscoveryStub::Instance()->startServiceDiscovery(rui_type, this );
    dumpServerListToConsole(serverList);
#else
// Start real discovery
#endif
}

void DiscoveryProxy::serverListUpdate(std::string type, UPnPDeviceList serverList)
{
    if ( type.compare(rui_type) != 0 ) {
        fprintf(stderr,"\nServer List Update: Invalid service type: %s\n", type.c_str());
    } else {
        dumpServerListToConsole(serverList);
    }
}

void DiscoveryProxy::dumpServerListToConsole(UPnPDeviceList serverList)
{
    fprintf(stderr,"\nServer List Update:\n");

    UPnPDeviceList::iterator p;

    for(p = serverList.begin(); p!=serverList.end(); ++p) {
        UPnPDevice device = p->second;
        fprintf(stderr,"RUI Server: %s -> %s\n", device.friendlyName.c_str(), device.host.c_str());
    }
}


