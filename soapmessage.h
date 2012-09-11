#ifndef SOAPMESSAGE_H
#define SOAPMESSAGE_H

#include <QObject>
#include <QDomDocument>
#include <QDomElement>


class SoapMessage : public QObject
{
    Q_OBJECT
public:
    explicit SoapMessage(QObject *parent = 0);
    void setMethod(const QString& action, const QString& attibute, const QString& uri);
    void addMethodArgument(const QString& name, const QString& value);
    void generateEnvelope();
    QString message();

signals:
    
public slots:

private:
    QDomDocument m_document;
    QDomElement m_body;
    QDomElement m_method;
};

#endif // SOAPMESSAGE_H
