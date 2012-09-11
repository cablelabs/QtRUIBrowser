#include "soapmessage.h"
#include <stdio.h>

#include <QDomText>

#define kElementEnvelope "s:Envelope"
#define kElementBody "s:Body"

SoapMessage::SoapMessage(QObject *parent) :
    QObject(parent), m_document()
{
    generateEnvelope();

}

/*
        var soapActionString = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                + "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                + "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                + "<s:Body>"
                + "<u:GetCompatibleUIs xmlns:u=\"urn:schemas-upnp-org:service:RemoteUIServer:1\">"
                + "<InputDeviceProfile/>"
                + "<UIFilter>\"*\"</UIFilter>"
                + "</u:GetCompatibleUIs>"
                + "</s:Body>"
                + "</s:Envelope>";
 **/

void SoapMessage::setMethod(const QString& method, const QString& xmlns, const QString& uri)
{
    // TODO: Remove existing method if present.
    m_method = m_document.createElement(method);
    m_method.setAttribute(xmlns,uri);
    m_body.appendChild(m_method);
}

void SoapMessage::addMethodArgument(const QString& name, const QString& value)
{
    // TODO: test for valid method

    QDomElement arg = m_document.createElement(name);
    if (value.length() > 0) {
        QDomText text = m_document.createTextNode(value);
        arg.appendChild(text);
    }
    m_method.appendChild(arg);

}

void SoapMessage::generateEnvelope()
{
    QDomElement root = m_document.createElement(kElementEnvelope);
    root.setAttribute("xmlns:s","http://schemas.xmlsoap.org/soap/envelope");
    root.setAttribute("s:encodingStyle","http://schemas.xmlsoap.org/soap/encoding");
    m_document.appendChild(root);

    m_body = m_document.createElement(kElementBody);
    root.appendChild(m_body);

    /*
    QDomElement tag = doc.createElement("Greeting");
    root.appendChild(tag);

    QDomText t = doc.createTextNode("Hello World");
    tag.appendChild(t);
    */


}

QString SoapMessage::message()
{
    QString xml = m_document.toString();
    return xml;
}
