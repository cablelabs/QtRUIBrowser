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
//#include <QWebView>
#include "qwebview.h"
#include "discoveryproxy.h"
#include "qwebinspector.h"
#include "webinspector.h"

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
    void home();
    void checkHttpProxyEnabled();

protected slots:
    void setAddressUrl(const QString& url);
    void setAddressUrl(const QUrl& url);
    void openLocation();
    void openFile();
    void changeLocation();
    void toggleNavigationBar(bool on);
    void toggleHttpProxy(bool on);
    void toggleWebInspector(bool on);
    void dumpUserInterfaceMap();
    void dumpHtml();
    void fullScreenOn();

    void onIconChanged();
    void onLoadStarted();
    void onTitleChanged(const QString&);
    void onPageLoaded(bool);
    void onJavaScriptWindowObjectCleared();

protected:
    QString addressUrl() const;
    //virtual void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject* object, QEvent* event);
    private:
    void buildUI();
    void init();
    void createMenuBar();
    void attachProxyObject();
    void enableHttpProxy();
    void fullScreen(bool on);


    QWebView* m_view;
    RUIWebPage* m_page;
    QToolBar* m_navigationBar;
    QStringListModel m_urlModel;
    QStringList m_urlList;
    LocationEdit* m_urlEdit;
    DiscoveryProxy* m_discoveryProxy;
    BrowserSettings* m_browserSettings;
    WebInspector* m_inspector;
};

#endif
