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
