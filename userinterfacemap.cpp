/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * userinterfacemap.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/12/2012.
 *
 * Description: Container for storing compatible user interfaces.
 *
 */

#include "userinterfacemap.h"
#include <QMap>
#include <stdio.h>


UserInterfaceMap::UserInterfaceMap(QObject *parent) :
    QObject(parent)
{
}

void UserInterfaceMap::addDevice(const RUIDevice& device) {

    m_deviceMap.insert(device.m_uuid, device);
}

int UserInterfaceMap::checkForRemovedDevices( const QStringList& newDeviceList )
{
    int deleteCount = 0;

    QStringList removeList;

    // Walk through our existing devices
    QMapIterator<QString, RUIDevice> i(m_deviceMap);
     while (i.hasNext()) {
         i.next();
         const RUIDevice device = i.value();
         QString uuid = device.m_uuid;

         // Locate in new device list.
         if (!newDeviceList.contains(uuid)) {

             removeList.append(uuid);
             deleteCount++;
         }
     }

     foreach (QString deviceUuid, removeList) {
        removeDevice(deviceUuid);
     }

     return deleteCount;
}

void UserInterfaceMap::removeDevice(const QString& uuid) {

    if (m_deviceMap.contains(uuid)) {

        RUIDevice device = m_deviceMap[uuid];


        QListIterator<RUIService> iterator(device.m_serviceList);

        while (iterator.hasNext()) {

            RUIService service = iterator.next();
            QString serviceKey = service.m_controlURL;
            removeServiceUIs(serviceKey);
        }
    }

    m_deviceMap.remove(uuid);
}

void UserInterfaceMap::addServiceUIs(const QString& serviceKey, const QList<RUIInterface>& uiList) {

    m_mutex.lock();
    m_serviceUIs.insert(serviceKey, uiList);
    m_mutex.unlock();
}

void UserInterfaceMap::removeServiceUIs(const QString& serviceKey) {

    m_mutex.lock();
    m_serviceUIs.remove(serviceKey);
    m_mutex.unlock();
}

void UserInterfaceMap::dumpToConsole()
{
    fprintf( stderr, "\nUserInterfaceMap:\n");
    QMapIterator<QString, RUIDevice> i(m_deviceMap);
     while (i.hasNext()) {
         i.next();
         const RUIDevice device = i.value();
         fprintf( stderr,"- Device: %s [%s]\n", device.m_friendlyName.toAscii().data(), device.m_uuid.toAscii().data());
         fprintf( stderr,"  - baseURL: %s\n", device.m_baseURL.toAscii().data());

         QListIterator<RUIService> iterator(device.m_serviceList);

         while (iterator.hasNext()) {

             RUIService service = iterator.next();
             QString serviceKey = service.m_controlURL;
             fprintf( stderr,"  - Service: %s\n", service.m_serviceID.toAscii().data());
             fprintf( stderr,"    - type: %s\n", service.m_serviceType.toAscii().data());
             fprintf( stderr,"    - baseURL: %s\n", service.m_baseURL.toAscii().data());
             fprintf( stderr,"    - eventURL: %s\n", service.m_eventURL.toAscii().data());
             fprintf( stderr,"    - controlURL: %s\n", service.m_controlURL.toAscii().data());
             fprintf( stderr,"    - descriptionURL: %s\n", service.m_descriptionURL.toAscii().data());

             if (m_serviceUIs.contains(serviceKey)) {

                 QList<RUIInterface> serviceList = m_serviceUIs[serviceKey];

                 QListIterator<RUIInterface> iterator(serviceList);

                 while (iterator.hasNext()) {

                     RUIInterface ui = iterator.next();

                     fprintf( stderr,"    - ui: %s [%s]\n", ui.m_name.toAscii().data(), ui.m_uiID.toAscii().data());
                     fprintf( stderr,"      - description: %s\n", ui.m_description.toAscii().data());

                     QListIterator<RUIIcon> iconIterator(ui.m_iconList);

                     while (iconIterator.hasNext()) {

                         RUIIcon icon = iconIterator.next();
                         fprintf( stderr,"      - icon: %sx%s (%s bit, %s) - %s\n",
                                  icon.m_width.toAscii().data(),
                                  icon.m_height.toAscii().data(),
                                  icon.m_depth.toAscii().data(),
                                  icon.m_mimeType.toAscii().data(),
                                  icon.m_url.toAscii().data());
                     }

                     QListIterator<RUIProtocol> protocolIterator(ui.m_protocolList);

                     while (protocolIterator.hasNext()) {

                         RUIProtocol protocol = protocolIterator.next();
                         fprintf( stderr,"      - protocol: %s (%s)\n",
                                  protocol.m_shortName.toAscii().data(),
                                  protocol.m_protocolInfo.toAscii().data());

                         QListIterator<QString> uriIterator(protocol.m_uriList);

                         while (uriIterator.hasNext()) {

                             QString uri = uriIterator.next();
                             fprintf( stderr,"        - uri: %s\n",
                                      uri.toAscii().data());
                         }
                     }

                 }
             }
         }
     }
}

QVariantList UserInterfaceMap::generateUIList()
{
    //fprintf( stderr, "generateUIList\n");
    QVariantList list;

    m_mutex.lock();

    QMapIterator<QString, QList<RUIInterface> > i(m_serviceUIs);
    while (i.hasNext()) {

        i.next();
        const QList<RUIInterface> interfaceList = i.value();

        QListIterator<RUIInterface> iterator(interfaceList);

        while (iterator.hasNext()) {

            RUIInterface interface = iterator.next();
            list.append( interface.toMap());
        }
    }

    m_mutex.unlock();

    return list;
}



