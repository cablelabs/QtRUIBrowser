/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * userinterfacemap.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/12/2012.
 *
 * Description: Tree for storing compatible user interfaces.
 *
 */

#ifndef USERINTERFACEMAP_H
#define USERINTERFACEMAP_H

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QVariantMap>
#include <QList>
#include <QStringList>
#include <QMutex>

/* The following support classes are used by the UserInterfaceMap API:
 * - RUIIcon
 * - RUIProtocol
 * - RUIInterface
 * - RUIService
 * - RUIDevice
 */

class RUIIcon : public QObject
{
public:

    RUIIcon() : QObject() {}
    RUIIcon(const RUIIcon& other) : QObject() { *this = other; }
    RUIIcon& operator=(const RUIIcon& other)
    {
        m_mimeType = other.m_mimeType;
        m_url = other.m_url;
        m_width = other.m_width;
        m_height = other.m_height;
        m_depth = other.m_depth;
        return *this;
    }

    QVariantMap toMap()
    {
        QVariantMap map;
        map["mimeType"] = m_mimeType;
        map["width"] = m_width;
        map["height"] = m_height;
        map["depth"] = m_depth;
        map["url"] = m_url;
        return map;
    }

    QString m_mimeType;
    QString m_width;
    QString m_height;
    QString m_depth;
    QString m_url;
};

class RUIProtocol : public QObject
{
public:

    RUIProtocol() : QObject() {}
    RUIProtocol(const RUIProtocol& other) : QObject() { *this = other; }
    RUIProtocol& operator=(const RUIProtocol& other)
    {
        m_shortName = other.m_shortName;
        m_protocolInfo = other.m_protocolInfo;
        m_uriList = other.m_uriList;
        return *this;
    }

    QVariantMap toMap()
    {
        QVariantMap map;
        map["shortName"] = m_shortName;
        map["protocolInfo"] = m_protocolInfo;
        map["uriList"] = m_uriList;
        return map;
    }

    QString m_shortName;
    QString m_protocolInfo;

    // A protocol can have multiple uris
    QStringList m_uriList;
};


class RUIInterface : public QObject
{
public:

    RUIInterface() : QObject() {}
    RUIInterface(const RUIInterface& other) : QObject() { *this = other; }
    RUIInterface& operator=(const RUIInterface& other)
    {
        m_uiID = other.m_uiID;
        m_name = other.m_name;
        m_description = other.m_description;
        m_iconList = other.m_iconList;
        m_protocolList = other.m_protocolList;
        return *this;
    }

    QVariantMap toMap()
    {
        QVariantMap map;
        map["uiID"] = m_uiID;
        map["name"] = m_name;
        map["description"] = m_description;
        QVariantList iconList;
        foreach (RUIIcon icon, m_iconList)
            iconList.append(icon.toMap());

        map["iconList"] = iconList;
        QVariantList protocolList;
        foreach (RUIProtocol protocol, m_protocolList)
            protocolList.append(protocol.toMap());

        map["protocolList"] = protocolList;
        return map;
    }

    QString m_uiID;
    QString m_name;
    QString m_description;

    // An interface can have multiple icons
    QList<RUIIcon> m_iconList;

    // An interface can have multiple protocols
    QList<RUIProtocol> m_protocolList;
};

// A RUI Service does not have a uuid (?!), so we use the full control URL for uniqueness.
// We don't store the UIs in the RUIService object because they are retrieved after we parse
// the device description, and the devices are stored in the device map as const. There is
// a separate map for each service containing a list of UIs.
class RUIService : public QObject
{
public:

    RUIService() : QObject() {}
    RUIService(const RUIService &other) : QObject() { *this = other; }
    RUIService& operator=(const RUIService& other)
    {
        m_serviceID = other.m_serviceID;
        m_serviceType = other.m_serviceType;
        m_baseURL = other.m_baseURL;
        m_eventURL = other.m_eventURL;
        m_controlURL = other.m_controlURL;
        m_descriptionURL = other.m_descriptionURL;
        return *this;
    }
    QString m_serviceID;
    QString m_serviceType;
    QString m_baseURL;
    QString m_eventURL;
    QString m_controlURL;
    QString m_descriptionURL;
};

// The RUIDevice class represents a RUIServer specific instance of a UPnP Device. Note that a device can
// be a sub device of a root device.
class RUIDevice : public QObject
{
public:

    RUIDevice() : QObject() {}
    RUIDevice(const RUIDevice& other) : QObject() { *this = other; }
    RUIDevice& operator=(const RUIDevice& other)
    {
        m_friendlyName = other.m_friendlyName;
        m_baseURL = other.m_baseURL;
        m_uuid = other.m_uuid;
        m_serviceList = other.m_serviceList;
        m_rootDeviceUuid = other.m_rootDeviceUuid;
        return *this;
    }

    QString m_friendlyName;
    QString m_baseURL;
    QString m_uuid;
    QString m_rootDeviceUuid;

    // We are only interested in a service of type urn:schemas-upnp-org:service:RemoteUIServer:1
    // There should be 0 or 1 instances of this service type per device, but allow for multiples.
    // We only store devices that support this service - all others are discarded.
    QList<RUIService> m_serviceList;
};

class UserInterfaceMap : public QObject
{
    Q_OBJECT

public:
    explicit UserInterfaceMap(QObject *parent = 0);

    void addDevice(const RUIDevice& device);
    void removeDevice(const QString& uuid);
    bool deviceExists(const QString& uuid);
    void addServiceUIs(const QString& serviceKey, const QList<RUIInterface>& list);
    void removeServiceUIs(const QString& serviceKey);
    int checkForRemovedDevices( const QStringList& newDeviceList );
    bool isHostRUITransportServer( const QString& hostURL );

    QVariantList generateUIList();

    // Debugging
    void dumpToConsole();

private:

    QMap<QString, RUIDevice> m_deviceMap;
    QMap<QString, QList<RUIInterface> > m_serviceUIs;
    QMutex m_mutex;
    QMap<QString, QString> m_transportServers;
    
signals:

public slots:
    
};

#endif // USERINTERFACEMAP_H
