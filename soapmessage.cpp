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
}

QString SoapMessage::message()
{
    QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    xml += m_document.toString();
    return xml;
}
