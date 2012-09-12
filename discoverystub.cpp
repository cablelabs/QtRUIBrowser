/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * discoverystub.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Testing stub to simulate the WebKit Discovery module
 *
 */

#include "discoverystub.h"
#include <QTimer>

DiscoveryStub* DiscoveryStub::m_pInstance = NULL;

DiscoveryStub::DiscoveryStub()
{
}

DiscoveryStub* DiscoveryStub::Instance()
{
    if ( !m_pInstance )
    {
        m_pInstance = new DiscoveryStub;
    }

    return m_pInstance;
}

UPnPDeviceList DiscoveryStub::startServiceDiscovery(std::string type, IDiscoveryAPI *api)
{
    m_type = type;
    m_discoveryAPI = api;
    m_listIndex = 0;

    // Start timer to update the server list every 10 seconds
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(updateServerList()));
    //timer->start(10000);

    // return initial list
    return generateServerList();
}

UPnPDeviceList DiscoveryStub::generateServerList()
{
    UPnPDeviceList list;

    addDevice(list,"http://localhost:2666/devdesc.xml","sky tv", "server1.com/event.xml","http://localhost:2666", "5001", "UUID0001" );

    /*
    if (m_listIndex == 0) {

        addDevice(list,"server1.com/description.xml","sky tv", "server1.com/event.xml","server1.com", "5001", "UUID0001" );
        addDevice(list,"server2.com/description.xml","apple", "server2.com/event.xml","server2.com", "5002", "UUID0002" );
        addDevice(list,"server3.com/description.xml","hulu", "server3.com/event.xml","server3.com", "5003", "UUID0003" );

    } else {

        addDevice(list,"server4.com/description.xml","videos", "server4.com/event.xml","server4.com", "5004", "UUID0004" );
        addDevice(list,"server5.com/description.xml","itunes", "server5.com/event.xml","server5.com", "5005", "UUID0005" );
        addDevice(list,"server6.com/description.xml","pixar", "server6.com/event.xml","server6.com", "5006", "UUID0006" );
        addDevice(list,"server7.com/description.xml","cablelabs", "server7.com/event.xml","server7.com", "5007", "UUID0007" );
        addDevice(list,"server8.com/description.xml","sony", "server8.com/event.xml","server8.com", "5008", "UUID0008" );
    }
    */

    m_listIndex = !m_listIndex;
    return list;
}

void DiscoveryStub::updateServerList()
{
    m_discoveryAPI->serverListUpdate(m_type, generateServerList());
}

void DiscoveryStub::addDevice(UPnPDeviceList& list, std::string descURL, std::string friendlyName, std::string eventURL, std::string host, std::string port, std::string uuid)
{
    UPnPDevice device;
    device.descURL = descURL;
    device.friendlyName = friendlyName;
    device.eventURL = eventURL;
    device.host = host;
    device.port = port;
    device.uuid = uuid;
    device.isOkToUse = true;

    list[uuid] = device;
}

