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
#ifndef DISCOVERYSTUB_H
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
