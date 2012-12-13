/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * browsersettings.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Runtime settings (qtruibrowser.ini)
 *
 */

#include "browsersettings.h"
#include <QFile>

BrowserSettings* BrowserSettings::m_pInstance = NULL;

#define keyTitleBar       "window/titleBar"
#define keyMenuBar        "window/menuBar"
#define keyNavigationBar  "window/navigationBar"
#define keyStartFullScreen "window/startFullScreen"
#define keyStaysOnTop     "window/staysOnTop"
#define keyWebInspector   "window/webInspector"

#define keyUrlEdit        "navigation/urlEdit"
#define keyHomeButton     "navigation/homeButton"
#define keyBackButton     "navigation/backButton"
#define keyReloadButton   "navigation/reloadButton"
#define keyForwardButton  "navigation/forwardButton"
#define keyStopButton     "navigation/stopButton"

#define keyProxyHost      "httpProxy/host"
#define keyProxyPort      "httpProxy/port"
#define keyProxyEnabled   "httpProxy/enabled"
#define keyProxyType      "httpProxy/type"

#define keyCertID         "server/certID"
#define keyTVRemoteUrl    "server/tvRemoteURL"

#define keyRUIUrl         "defaultRUI/url"
#define keyRUIImage       "defaultRUI/image"
#define keyRUILabel       "defaultRUI/label"

BrowserSettings::BrowserSettings(QObject *parent) :
    QSettings("qtruibrowser.ini", QSettings::IniFormat, parent)
{
    generateDefaults();

    if (contains(keyTitleBar)) hasTitleBar = value(keyTitleBar).toBool();
    if (contains(keyMenuBar)) hasMenuBar = value(keyMenuBar).toBool();
    if (contains(keyNavigationBar)) hasNavigationBar = value(keyNavigationBar).toBool();
    if (contains(keyStartFullScreen)) startFullScreen = value(keyStartFullScreen).toBool();
    if (contains(keyStaysOnTop)) staysOnTop = value(keyStaysOnTop).toBool();
    if (contains(keyWebInspector)) hasWebInspector = value(keyWebInspector).toBool();

    if (contains(keyUrlEdit)) hasUrlEdit = value(keyUrlEdit).toBool();
    if (contains(keyHomeButton)) hasHomeButton = value(keyHomeButton).toBool();
    if (contains(keyBackButton)) hasBackButton = value(keyBackButton).toBool();
    if (contains(keyReloadButton)) hasReloadButton = value(keyReloadButton).toBool();
    if (contains(keyForwardButton)) hasForwardButton = value(keyForwardButton).toBool();
    if (contains(keyStopButton)) hasStopButton = value(keyStopButton).toBool();

    if (contains(keyCertID)) certID = value(keyCertID).toString();
    if (contains(keyTVRemoteUrl)) tvRemoteURL = value(keyTVRemoteUrl).toString();

    if (contains(keyRUIUrl)) defaultRUIUrl = value(keyRUIUrl).toString();
    if (contains(keyRUIImage)) defaultRUIImage = value(keyRUIImage).toString();
    if (contains(keyRUILabel)) defaultRUILabel = value(keyRUILabel).toString();

    if (contains(keyProxyHost)) proxyHost = value(keyProxyHost).toString();
    if (contains(keyProxyPort)) proxyPort = value(keyProxyPort).toInt();
    if (contains(keyProxyEnabled)) proxyEnabled = value(keyProxyEnabled).toBool();
    if (contains(keyProxyType)) proxyType = value(keyProxyType).toString();

    save();
}

void BrowserSettings::generateDefaults()
{
    hasTitleBar = true;
    hasMenuBar = true;
    hasNavigationBar = true;
    startFullScreen = false;
    staysOnTop = false;

    hasUrlEdit = true;
    hasHomeButton = true;
    hasBackButton = true;
    hasReloadButton = true;
    hasForwardButton = true;
    hasStopButton = true;
    hasWebInspector = false;

    certID = "certIdPlaceHolder";
    tvRemoteURL = "http://localhost/remote.html";

    defaultRUIUrl = "";
    defaultRUIImage = "";
    defaultRUILabel = "";

    proxyEnabled = false;
    proxyHost = "127.0.1.1";
    proxyPort = 8888;   // Charles Web Proxy
    proxyType = "HTTP";
}

void BrowserSettings::save()
{
    setValue(keyTitleBar, hasTitleBar );
    setValue(keyMenuBar, hasMenuBar);
    setValue(keyNavigationBar, hasNavigationBar);
    setValue(keyStartFullScreen, startFullScreen);
    setValue(keyStaysOnTop, staysOnTop);

    setValue(keyUrlEdit, hasUrlEdit);
    setValue(keyHomeButton, hasHomeButton);
    setValue(keyBackButton, hasBackButton);
    setValue(keyReloadButton, hasReloadButton);
    setValue(keyForwardButton, hasForwardButton);
    setValue(keyStopButton, hasStopButton);
    setValue(keyWebInspector, hasWebInspector);

    setValue(keyCertID, certID);
    setValue(keyTVRemoteUrl, tvRemoteURL);

    setValue(keyRUIUrl, defaultRUIUrl);
    setValue(keyRUIImage, defaultRUIImage);
    setValue(keyRUILabel, defaultRUILabel);

    setValue(keyProxyEnabled, proxyEnabled);
    setValue(keyProxyHost, proxyHost);
    setValue(keyProxyPort, proxyPort);
    setValue(keyProxyType, proxyType);
}

BrowserSettings* BrowserSettings::Instance()
{
    if ( !m_pInstance )
    {
        m_pInstance = new BrowserSettings;
    }

    return m_pInstance;
}
