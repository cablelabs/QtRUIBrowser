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
#ifndef ruiwebpage_h
#define ruiwebpage_h

#include <qwebframe.h>
#include <qwebpage.h>
#include <QLibrary>

class RUIWebPage : public QWebPage {
    Q_OBJECT

public:
    RUIWebPage(QObject* parent = 0);

    QString userAgentForUrl(const QUrl& url) const;
    bool clientAuthExtensionReceived;
    bool serverAuthExtensionReceived;
    QByteArray suppDataToSend;
    QSslCertificate suppDataX509;
    QByteArray nonce;
    QByteArray formatDTCPSuppData(QNetworkReply *reply);
    QLibrary dtcp;

    typedef int (*DTCPGetLocalCert)(unsigned char *, unsigned int *);
    typedef int (*DTCPVerifyRemoteCert)(unsigned char *);
    typedef int (*DTCPSignData)(unsigned char *, unsigned int, unsigned char *, unsigned int*);
    typedef int (*DTCPVerifyData)(unsigned char *, unsigned int, unsigned char *, unsigned char*);
    typedef int (*DTCPInit)(char *);

private slots:
    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);
    void setTlsExtension(quint16 extensionType, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply);
    void setTlsSupplementalDataEntry(quint16 supplementalDataType, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply);
    void receiveTlsExtension(const QTlsExtension &tlsExtension, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply);
    void receiveTlsSupplementalDataEntry(const QTlsSupplementalDataEntry &supplementalDataEntry, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply);
};

#endif
