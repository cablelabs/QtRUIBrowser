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
#include "ruiwebpage.h"
#include "qwebpage.h"
#include "discoveryproxy.h"
#include "browsersettings.h"

#include <QMessageBox>
#include <QNetworkReply>
#include <QSslError>
#include <QtDebug>
#include <QLibrary>
#include <openssl/ssl.h>

RUIWebPage::RUIWebPage(QObject* parent)
    : QWebPage(parent)
      , clientAuthExtensionReceived(false)
      , serverAuthExtensionReceived(false)
      , suppDataToSend(0)
      , suppDataX509(0)
{
    connect(networkAccessManager(),
        SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> &)), this,
        SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> &)));  

    //load DTCP library and resolve functions
    dtcp.setFileName(qgetenv("DTCP_LIBRARY_PATH").data());
    dtcp.load();
    if (!dtcp.isLoaded()) {
        qDebug() << "Unable to load DTCP library from DTCP_LIBRARY_PATH environment variable";
        return;
    }

    DTCPGetLocalCert getLocalCert = (DTCPGetLocalCert)dtcp.resolve("CVP2_DTCPIP_GetLocalCert");
    if (!getLocalCert) {
        qDebug() << "Unable to resolve CVP2_DTCPIP_GetLocalCert function from library: " << dtcp.fileName();
        return;
    }

    DTCPVerifyRemoteCert verifyRemoteCert = (DTCPVerifyRemoteCert)dtcp.resolve("CVP2_DTCPIP_VerifyRemoteCert");
    if (!verifyRemoteCert) {
        qDebug() << "Unable to resolve CVP2_DTCPIP_VerifyRemoteCert function from library: " << dtcp.fileName();
        return;
    }

    DTCPSignData signData = (DTCPSignData)dtcp.resolve("CVP2_DTCPIP_SignData");
    if (!signData) {
        qDebug() << "Unable to resolve CVP2_DTCPIP_SignData function from library: " << dtcp.fileName();
        return;
    }

    DTCPVerifyData verifyData = (DTCPVerifyData)dtcp.resolve("CVP2_DTCPIP_VerifyData");
    if (!verifyData) {
        qDebug() << "Unable to resolve CVP2_DTCPIP_VerifyData function from library: " << dtcp.fileName();
        return;
    }

    DTCPInit dtcpInit = (DTCPInit)dtcp.resolve("CVP2_DTCPIP_Init");
    if (!dtcpInit) {
        qDebug() << "Unable to resolve CVP2_DTCPIP_Init function from library: " << dtcp.fileName();
        return;
    }

    //init return of zero is success
    //init return of zero is success
    int initResult = dtcpInit(qgetenv("DTCP_KEY_STORAGE_DIR").data());
    if (initResult) {
        qDebug() << "DTCP initialization failed - unable to initialize DTCP library using keys from DTCP_KEY_STORAGE_DIR environment variable - error code: " << initResult;
        return;
    }

    //set the default SSL configuration to handle client_authz and server_authz TLS extensions,
    //as well as authz_data TLS supplemental data type
    QSslConfiguration sslConfiguration(QSslConfiguration::defaultConfiguration());
    sslConfiguration.registerTlsExtensionType(TLSEXT_TYPE_client_authz);
    sslConfiguration.registerTlsExtensionType(TLSEXT_TYPE_server_authz);
    sslConfiguration.registerTlsSupplementalDataType(TLSEXT_SUPPLEMENTALDATATYPE_authz_data);
    QSslConfiguration::setDefaultConfiguration(sslConfiguration);

    connect(networkAccessManager(),
            SIGNAL(readyForTlsExtension(quint16, const QSharedPointer<QSslContext> &,
                                        QNetworkReply *)), this,
            SLOT(setTlsExtension(quint16, const QSharedPointer<QSslContext>, QNetworkReply *)));

    connect(networkAccessManager(),
            SIGNAL(receiveTlsExtension(const QTlsExtension &, const QSharedPointer<QSslContext> &,
                                       QNetworkReply *)), this,
            SLOT(receiveTlsExtension(QTlsExtension, const QSharedPointer<QSslContext>, QNetworkReply *)));

    connect(networkAccessManager(),
            SIGNAL(readyForTlsSupplementalDataEntry(quint16, const QSharedPointer<QSslContext> &,
                                                    QNetworkReply *)), this,
            SLOT(setTlsSupplementalDataEntry(quint16, const QSharedPointer<QSslContext>, QNetworkReply *)));

    connect(networkAccessManager(),
            SIGNAL(receiveTlsSupplementalDataEntry(const QTlsSupplementalDataEntry &,
                                                   const QSharedPointer<QSslContext> &,
                                                   QNetworkReply *)), this,
            SLOT(receiveTlsSupplementalDataEntry(QTlsSupplementalDataEntry, const QSharedPointer<QSslContext>,
                                                 QNetworkReply *)));
}

void RUIWebPage::setTlsExtension(quint16 extensionType, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply)
{
    Q_UNUSED(reply);
    qDebug() << "setTlsExtension - type " << extensionType;
    QByteArray array;

    quint8 authFormatType = 66; //TLSEXT_AUTHZDATAFORMAT_dtcp
    array.append(authFormatType);

    QTlsExtension extension(extensionType, array);

    sslContext->addTlsExtension(extension);
}

void RUIWebPage::receiveTlsExtension(const QTlsExtension &tlsExtension, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply)
{
    Q_UNUSED(reply);
    Q_UNUSED(sslContext);
    qDebug() << "TLS extension received: " << tlsExtension;

    if (tlsExtension.extensionType() == TLSEXT_TYPE_client_authz)
    {
        clientAuthExtensionReceived = tlsExtension.content().size() == 2 &&
                tlsExtension.content().at(1) == (char)TLSEXT_AUTHZDATAFORMAT_dtcp;
    }
    if (tlsExtension.extensionType() == TLSEXT_TYPE_server_authz)
    {
        serverAuthExtensionReceived = tlsExtension.content().size() == 2 &&
                tlsExtension.content().at(1) == (char)TLSEXT_AUTHZDATAFORMAT_dtcp;
    }
}

void RUIWebPage::receiveTlsSupplementalDataEntry(const QTlsSupplementalDataEntry &tlsSupplementalDataEntry, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply)
{
    Q_UNUSED(reply);

    qDebug() << "supplementalDataReceived";
    qDebug() << "client received supplemental data " << tlsSupplementalDataEntry;

    if (tlsSupplementalDataEntry.supplementalDataType() == TLSEXT_SUPPLEMENTALDATATYPE_authz_data)
    {
        //fields, in order:
        //required: 32-byte nonce
        //2-byte dtcp cert length (may be zero)
        //2-byte x509 cert length (may be zero)
        //if DTCP sent, X509 is also sent
        //required: 40-byte signature
        QByteArray dtcpSuppData = tlsSupplementalDataEntry.content();
        qDebug() << "suppdata entry content: " << dtcpSuppData.toHex();
        //skip type + length
        dtcpSuppData = dtcpSuppData.remove(0, 3);
        nonce = dtcpSuppData.left(32);
        //include the nonce in the signed data to verify
        QByteArray signedData = dtcpSuppData;
        dtcpSuppData = dtcpSuppData.remove(0, 32);
        qDebug() << "received server nonce: " << nonce.toHex();
        //if dtcpLength > 0, both DTCP and x509 were sent by server
        int dtcpLength = ((dtcpSuppData[0] & 0xFF) << 8) | (dtcpSuppData[1] & 0xFF);
        qDebug() << "client - received dtcplength: " << dtcpLength;
        if (dtcpLength > 0)
        {
            //normally used by DTCP validation
            QByteArray remoteCert = dtcpSuppData.mid(2, dtcpLength);

            int x509Length = ((dtcpSuppData[dtcpLength + 2] & 0xFF) << 8) | (dtcpSuppData[dtcpLength+ 2 +1] & 0xFF);
            qDebug() << "client - received x509length: " << x509Length;
            QByteArray x509Data = dtcpSuppData.mid((dtcpLength +2 + 2), x509Length);

            suppDataX509 = QSslCertificate(x509Data, QSsl::Der);
            qDebug() << "client - received x509 cert: " << x509Data.toHex();

            //must validate suppDataX509 with peer cert after handshake is complete
            //normally used by DTCP validation
            QByteArray signature = dtcpSuppData.right(40);
            int returnCode;
            //validate signature
            qDebug() << "verifying - size: " << signedData.size() << ", signed data: " << signedData.toHex();
            qDebug() << "verifying - signature: " << signature.toHex();
            qDebug() << "verifying - remote cert: " << remoteCert.toHex();

            DTCPVerifyData verifyData = (DTCPVerifyData)dtcp.resolve("CVP2_DTCPIP_VerifyData");
            returnCode = verifyData(reinterpret_cast<unsigned char*>(signedData.data()), signedData.size() - 40,
                                                reinterpret_cast<unsigned char*>(signature.data()),
                                                reinterpret_cast<unsigned char*>(remoteCert.data()));
            if (returnCode == 0)
            {
                qDebug() << "validation of signature succeeded";
            }
            else
            {
                qDebug() << "validation failed: invalid signature type - return code: " << returnCode << ", signature: " << signature.toHex();
            }

            DTCPVerifyRemoteCert verifyRemoteCert = (DTCPVerifyRemoteCert)dtcp.resolve("CVP2_DTCPIP_VerifyRemoteCert");
            returnCode = verifyRemoteCert(reinterpret_cast<unsigned char*>(remoteCert.data()));
            if (returnCode == 0)
            {
                qDebug() << "validation of cert succeeded";
            }
            else
            {
                qDebug() << "validation failed: invalid cert - return code: " << returnCode;
                QSslError error = QSslError(QSslError::UnsupportedExtension);
                sslContext->abortTlsHandshake(error);
            }
        }
    }
}

void RUIWebPage::setTlsSupplementalDataEntry(quint16 supplementalDataType, const QSharedPointer<QSslContext> &sslContext, QNetworkReply *reply)
{
    Q_UNUSED(reply);

    qDebug() << "in setTlsSupplementalDataEntry - clientAuthExtensionReceived: " << clientAuthExtensionReceived \
             << ", server auth extension received: " << serverAuthExtensionReceived;
    if (clientAuthExtensionReceived || serverAuthExtensionReceived)
    {
        if (!clientAuthExtensionReceived && !serverAuthExtensionReceived)
        {
            //only one extension received - abort
            QSslError error = QSslError(QSslError::UnsupportedExtension);
            sslContext->abortTlsHandshake(error);
            return;
        }
    } else {
        //no extensions received
        return;
    }

    suppDataToSend = formatDTCPSuppData(reply);
    if (!suppDataToSend.isNull())
    {
        QTlsSupplementalDataEntry entry(supplementalDataType, suppDataToSend);
        qDebug() << "sending client supplemental data: " << entry;
        sslContext->addTlsSupplementalDataEntry(entry);
    }
}

QByteArray RUIWebPage::formatDTCPSuppData(QNetworkReply *reply)
{
    // suppdate array has already been allocated by caller

    printf ("###########################################################\n");
    printf ("formatDTCPSuppData\n");

    QByteArray suppData = QByteArray();
    int nReturnCode = 0;
    unsigned char pLocalCert[132];
    unsigned int uLocalCertSize = 132;
    unsigned char pSignature[40];
    unsigned int uSignatureSize = 40;
    int index = 0;
    unsigned int pSignOffset = 0;
    unsigned int numBytesToSign;
    unsigned short encodedLength = 0;
    qDebug() << "format dtcp suppdata - setting nonce " << nonce.toHex();
    //supplementaldataentry object prefixes the type, no need to add here
    suppData.append(TLSEXT_AUTHZDATAFORMAT_dtcp);
    //placeholder for size
    suppData.append((char)(0));
    suppData.append((char)(0));
    index  += 3;
    //include nonce
    pSignOffset = index;
    numBytesToSign = 32;
    suppData.append(nonce);
    index += 32;

    // add local DTCP cert to supp data
    DTCPGetLocalCert getLocalCert = (DTCPGetLocalCert)dtcp.resolve("CVP2_DTCPIP_GetLocalCert");
    nReturnCode = getLocalCert (pLocalCert, &uLocalCertSize);
    printf("CVP2_DTCPIP_GetLocalCert returned %d\n", nReturnCode);
    if (nReturnCode != 0)
    {
        printf ("###########################################################\n");
        return NULL;
    }

/*    printf("LocalCert:\n");
    printf("uLocalCertSize = %d\n", uLocalCertSize);
    for (i=0; i<uLocalCertSize; i++)
    {
        printf ("0x%02x ", pLocalCert[i]);
        if (i%8 == 7)
        {
            printf ("\n");
        }
    }
    printf ("\n");
    */
    //copy size

    numBytesToSign += 2 + uLocalCertSize;

    int firstDTCPLenByte = (uLocalCertSize >> 8) & 0xFF;
    int secondDTCPLenByte = (uLocalCertSize & 0xFF);
    qDebug() << "Adding dtcp cert size: " << uLocalCertSize;
    suppData.append(firstDTCPLenByte);
    suppData.append(secondDTCPLenByte);
    index += 2;
    suppData.append(reinterpret_cast<const char*>(pLocalCert), uLocalCertSize);
    qDebug() << "Adding dtcp cert";
    index += uLocalCertSize;

    QSslCertificate x509;
    x509 = reply->sslConfiguration().localCertificate();
    if (!x509.isNull())
    {
        qDebug() << "adding local x509 certificate to supplemental data";
        QByteArray x509Der = x509.toDer();
        int x509Length = x509Der.length();
        int firstX509LenByte = ((x509Length >> 8) & 0xFF);
        int secondX509LenByte = x509Length & 0xFF;
        suppData.append(firstX509LenByte);
        suppData.append(secondX509LenByte);
        index += 2;
        suppData.append(x509Der.data(), x509Length);
        index += x509Length;
        numBytesToSign += 2 + x509Length;
        qDebug() << "client - sent cert: " << x509Der.toHex();
    }
    else
    {
        suppData.append((char)0);
        suppData.append((char)0);
        index += 2;
        numBytesToSign += 2;

        qDebug() << "no local x509 certificate - not adding x509 certificate to supplemental data";
    }

    // add signature of local DTCP cert to supp data
    DTCPSignData signData = (DTCPSignData)dtcp.resolve("CVP2_DTCPIP_SignData");
    nReturnCode =  signData(reinterpret_cast<unsigned char*>(suppData.data()) + pSignOffset, numBytesToSign, pSignature,
        &uSignatureSize);
    printf("CVP2_DTCPIP_SignData returned %d\n", nReturnCode);

    if (nReturnCode != 0)
    {
        printf ("###########################################################\n");
        return NULL;
    }

/*    printf("Signature:\n");
    printf("uSignatureSize = %d\n", uSignatureSize);
    for (i=0; i<uSignatureSize; i++)
    {
        printf ("0x%02x ", pSignature[i]);
        if (i%8 == 7)
        {
            printf ("\n");
        }
    }
    printf ("\n");
    */
    suppData.append(reinterpret_cast<const char*>(pSignature), uSignatureSize);

    index += uSignatureSize;

    encodedLength = index - 3;
    //update size 2-byte placeholder at index 1 of suppData
    suppData.data()[1]  = encodedLength >> 8 & 0xff;
    suppData.data()[2]  = encodedLength & 0xff;

    qDebug() << "Generate suppData - length: " << suppData.length() << ", data: " << suppData.toHex();
    printf ("###########################################################\n");

    return suppData;
}

QString RUIWebPage::userAgentForUrl(const QUrl& url) const
{
    QString userAgent = QWebPage::userAgentForUrl(url);

    QString scheme = url.scheme();
    QString host = url.host();

    DiscoveryProxy* proxy = DiscoveryProxy::Instance();
    BrowserSettings* settings = BrowserSettings::Instance();

    // Always add the product token, but only add the CertID if this is a RUI Transport Server
    // AND the protocol is https.
    userAgent += " DLNADOC/1.50 DLNA-HTML5/1.0";
    if (scheme.compare("https") == 0) {
        if (proxy->isHostRUITransportServer(host)) {
            userAgent += " (CertID " + settings->certID + ")";
        }
    }

    return userAgent;
}

void RUIWebPage::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    QStringList errorMessages;
    foreach (QSslError e, errors) {
        errorMessages += e.errorString();
    }

    QString text = "The following SSL errors have occurred:\n\n" +
        errorMessages.join("\n") +
        "\n\nYour connection to this site is not secure. Do you want to continue anyway?";
    QMessageBox box(QMessageBox::Critical, "SSL Errors", text,
        QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);
    if (box.exec() == QMessageBox::Yes) {
        reply->ignoreSslErrors(errors);
    }
}
