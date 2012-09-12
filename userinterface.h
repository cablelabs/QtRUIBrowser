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
};

#endif // USERINTERFACE_H
