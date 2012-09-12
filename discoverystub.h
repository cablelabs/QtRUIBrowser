#ifndef DISCOVERYSTUB_H
/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * discoverystub.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Testing stub to simulate the WebKit Discovery module
 *
 */

#define DISCOVERYSTUB_H

#include <QObject>
#include <string>
#include <map>

//#include ""

/* Temporary stub for testing.
 *
 */

typedef struct sUPnPDevice
{
    std::string descURL;
    std::string friendlyName;
    std::string eventURL;
    std::string host;
    std::string port;
    std::string uuid;
    bool isOkToUse;
} UPnPDevice;

typedef std::map<std::string, UPnPDevice> UPnPDeviceList;

// Interface declarations
class IDiscoveryAPI
{
public:
    virtual void serverListUpdate(std::string type, std::map<std::string, UPnPDevice> devs) = 0;
};

class UPnPSearch
{
public:
    virtual UPnPDeviceList startServiceDiscovery(std::string, IDiscoveryAPI *api) = 0;
};


class DiscoveryStub : public QObject, public UPnPSearch
{
    Q_OBJECT

public:
    DiscoveryStub();
    static DiscoveryStub* Instance();

    // UPnPSearch interface
    virtual UPnPDeviceList startServiceDiscovery(std::string type, IDiscoveryAPI *api);

    UPnPDeviceList generateServerList();

private:
    static DiscoveryStub* m_pInstance;
    std::string m_type;
    IDiscoveryAPI* m_discoveryAPI;
    int m_listIndex;

    void addDevice(UPnPDeviceList& list, std::string descURL, std::string friendlyName, std::string eventURL, std::string host, std::string port, std::string uuid);

public slots:

    void updateServerList();

};

#endif // DISCOVERYSTUB_H
