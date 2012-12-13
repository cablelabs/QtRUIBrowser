/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * browsersettings.h
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Runtime settings (qtruibrowser.ini)
 *
 */

#ifndef BROWSERSETTINGS_H
#define BROWSERSETTINGS_H

#include <QSettings>

class BrowserSettings : public QSettings
{
    Q_OBJECT
public:
    explicit BrowserSettings(QObject *parent = 0);
    static BrowserSettings* Instance();
    
signals:
    
public slots:

private:
    static BrowserSettings* m_pInstance;
    void generateDefaults();

public:
    bool hasTitleBar;
    bool hasMenuBar;
    bool hasNavigationBar;
    bool hasUrlEdit;
    bool hasHomeButton;
    bool hasBackButton;
    bool hasForwardButton;
    bool hasReloadButton;
    bool hasStopButton;
    bool hasWebInspector;
    bool startFullScreen;
    bool staysOnTop;
    bool proxyEnabled;
    int  proxyPort;
    QString proxyType;
    QString proxyHost;
    QString certID;
    QString defaultRUIUrl;
    QString defaultRUIImage;
    QString defaultRUILabel;
    QString tvRemoteURL;
    void save();

};

#endif // BROWSERSETTINGS_H
