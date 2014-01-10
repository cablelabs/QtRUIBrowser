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

static const int TIMEOUT_SECONDS = 10;

RUIWebPage::RUIWebPage(QObject* parent)
    : QWebPage(parent)
{
    m_loadTimer.setInterval(TIMEOUT_SECONDS * 1000);
    m_loadTimer.setSingleShot(true);

    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(handleLoadFinished(bool)));
    connect(this, SIGNAL(loadStarted()), this, SLOT(handleLoadStarted()));
    connect(networkAccessManager(),
        SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> &)), this,
        SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> &)));
    connect(&m_loadTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
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

void RUIWebPage::handleLoadFinished(bool ok)
{
    qDebug() << "Load" << (ok ? "successful" : "failed");
    m_loadTimer.stop();
}

void RUIWebPage::handleLoadStarted()
{
    qDebug() << "Started page load. Setting timeout for " << TIMEOUT_SECONDS << " seconds.";
    m_loadTimer.start();
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
        QMessageBox::Yes | QMessageBox::No, view());
    box.setDefaultButton(QMessageBox::No);
    if (box.exec() == QMessageBox::Yes) {
        reply->ignoreSslErrors(errors);
    }
}

void RUIWebPage::handleTimeout()
{
    qDebug() << "Timeout reached, cancelling page load.";
    triggerAction(Stop, false);
    QMessageBox::warning(view(), "Page Load Timeout", "Page took too long to load.");
}
