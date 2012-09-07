/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * ruiwebpage.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Subclassed QWebPage to provide for the addition of a product token to the useragent header
 *
 */

#ifndef ruiwebpage_h
#define ruiwebpage_h

#include <qwebframe.h>
#include <qwebpage.h>

class RUIWebPage : public QWebPage {

    Q_OBJECT

public:
    RUIWebPage(QObject* parent = 0);

    //virtual bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);

    QString userAgentForUrl(const QUrl& url) const;

public slots:

private:

};

#endif
