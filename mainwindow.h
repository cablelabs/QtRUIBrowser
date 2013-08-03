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
#ifndef mainwindow_h
#define mainwindow_h

#include <QMainWindow>
#include <QStringListModel>
#include <QToolBar>
#include <QWebView>
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
    MainWindow(bool startFullScreen);

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
    bool eventFilter(QObject* object, QEvent* event);

private:
    void buildUI();
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
