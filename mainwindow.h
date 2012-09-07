/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * mainwindow.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Main application window for QtRUIBrowser
 *
 */

#ifndef mainwindow_h
#define mainwindow_h

#include <QMainWindow>
#include <QStringListModel>
#include <QToolBar>
#include <QWebView>
#include "discoveryproxy.h"

class LocationEdit;
class TVRemoteBridge;
class BrowserSettings;
class RUIWebPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

    void addCompleterEntry(const QUrl& url);

    void load(const QString& url);
    void load(const QUrl& url);

    RUIWebPage* page() const;
    void setPage(RUIWebPage*);
    void home();

protected slots:
    void setAddressUrl(const QString& url);
    void setAddressUrl(const QUrl& url);
    //void openFile();
    void openLocation();
    void changeLocation();
    void toggleTVRemote(bool on);
    void toggleNavigationBar(bool on);

    void onIconChanged();
    void onLoadStarted();
    void onTitleChanged(const QString&);

protected:
    QString addressUrl() const;
    virtual void keyPressEvent(QKeyEvent *event);
    //bool eventFilter(QObject* object, QEvent* event);
    private:
    void buildUI();
    void init();
    void createMenuBar();

    QWebView* m_view;
    QWebView* m_tvRemoteView;
    RUIWebPage* m_page;
    QWebPage* m_pageRemote;
    QToolBar* m_navigationBar;
    QStringListModel m_urlModel;
    QStringList m_urlList;
    LocationEdit* m_urlEdit;
    TVRemoteBridge* m_tvRemoteBridge;
    BrowserSettings* m_browserSettings;
    DiscoveryProxy* m_discoveryProxy;
};

#endif
