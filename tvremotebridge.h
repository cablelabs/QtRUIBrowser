/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * tvremotebridge.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: JavaScript bridge for the TV Remote page to marshal key presses to the navigation page
 *
 */

#ifndef TVREMOTEBRIDGE_H
#define TVREMOTEBRIDGE_H

#include <QWebPage>

class MainWindow;

class TVRemoteBridge : public QObject {
    Q_OBJECT

public:
    TVRemoteBridge( MainWindow* mainWindow, QWebPage* pageMain, QWebPage* pageRemote );

private:

    MainWindow* m_mainWindow;
    QWebPage* m_pageMain;
    QWebPage* m_pageRemote;

public slots:

    void remoteButtonPressed( const QString &param );
    void attachObject();

};

#endif // TVREMOTE_H
