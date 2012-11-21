/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * ruiwebpage.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Subclassed QWebPage to provide for the addition of a product token to the useragent header
 *
 */

#include "ruiwebpage.h"
//#include <QWebPage>
#include "qwebpage.h"
#include <stdio.h>
#include "discoveryproxy.h"
#include "browsersettings.h"

RUIWebPage::RUIWebPage(QObject* parent)
    : QWebPage(parent)
{
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

    if ( scheme.compare("https") == 0) {

        if (proxy->isHostRUITransportServer(host)) {

            userAgent += " (CertID " + settings->certID + ")";
        }
    }

    /*
    fprintf(stderr,"url: %s **  scheme: %s**  host: %s userAgent: %s\n"
            , url.toString().toAscii().data()
            , scheme.toAscii().data()
            , host.toAscii().data()
            , userAgent.toAscii().data()
            );
    */

    return userAgent;
}
