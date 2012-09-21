/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * userinterface.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/12/2012.
 *
 * Description: User Interface Descriptor
 *
 */

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>

class UserInterface : public QObject
{
    Q_OBJECT
public:
    explicit UserInterface(QObject *parent = 0);

    QString m_uuid;
    QString m_name;
    QString m_description;
    QString m_uri;

    // TODO - icon array
};

#endif // USERINTERFACE_H
