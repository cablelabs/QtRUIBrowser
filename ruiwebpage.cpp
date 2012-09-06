
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

    fprintf(stderr,"url: %s **  scheme: %s**  host: %s\n"
            , url.toString().toAscii().data()
            , scheme.toAscii().data()
            , host.toAscii().data()
            );

    return userAgent;
}
