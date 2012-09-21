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
#include <QWebPage>
#include <stdio.h>

RUIWebPage::RUIWebPage(QObject* parent)
    : QWebPage(parent)
{
}

QString RUIWebPage::userAgentForUrl(const QUrl& url) const
{
    QString userAgent = QWebPage::userAgentForUrl(url);

    QString scheme = url.scheme();
    QString host = url.host();

    /*
    fprintf(stderr,"url: %s **  scheme: %s**  host: %s\n"
            , url.toString().toAscii().data()
            , scheme.toAscii().data()
            , host.toAscii().data()
            );
            */

    return userAgent;
}
