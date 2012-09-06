#include "browsersettings.h"
#include <QFile>

BrowserSettings* BrowserSettings::m_pInstance = NULL;

#define keyTitleBar       "window/titleBar"
#define keyMenuBar        "window/menuBar"
#define keyNavigationBar  "window/navigationBar"
#define keyTVRemote       "window/tvRemote"
#define keyStartMaximized "window/startMaximized"
#define keyStaysOnTop     "window/staysOnTop"

#define keyUrlEdit        "navigation/urlEdit"
#define keyHomeButton     "navigation/homeButton"
#define keyBackButton     "navigation/backButton"
#define keyReloadButton   "navigation/reloadButton"
#define keyForwardButton  "navigation/forwardButton"
#define keyStopButton     "navigation/stopButton"

#define keyCertID         "server/certID"
#define keyTVRemoteUrl    "server/tvRemoteURL"

#define keyRUIUrl         "defaultRUI/url"
#define keyRUIImage       "defaultRUI/image"
#define keyRUILabel       "defaultRUI/label"

BrowserSettings::BrowserSettings(QObject *parent) :
    QSettings("qtruibrowser.ini", QSettings::IniFormat, parent)
{
    QFile settingsFile(this->fileName());
    if (!settingsFile.exists()) {
        generateDefaults();
        save();
    }

    hasTitleBar = value(keyTitleBar).toBool();
    hasMenuBar = value(keyMenuBar).toBool();
    hasNavigationBar = value(keyNavigationBar).toBool();
    hasTVRemote = value(keyTVRemote).toBool();
    startMaximized = value(keyStartMaximized).toBool();
    staysOnTop = value(keyStaysOnTop).toBool();

    hasUrlEdit = value(keyUrlEdit).toBool();
    hasHomeButton = value(keyHomeButton).toBool();
    hasBackButton = value(keyBackButton).toBool();
    hasReloadButton = value(keyReloadButton).toBool();
    hasForwardButton = value(keyForwardButton).toBool();
    hasStopButton = value(keyStopButton).toBool();

    certID = value(keyCertID).toString();
    tvRemoteURL = value(keyTVRemoteUrl).toString();

    defaultRUIUrl = value(keyRUIUrl).toString();
    defaultRUIImage = value(keyRUIImage).toString();
    defaultRUILabel = value(keyRUILabel).toString();
}

void BrowserSettings::generateDefaults()
{
    hasTitleBar = true;
    hasMenuBar = true;
    hasNavigationBar = true;
    hasTVRemote = false;
    startMaximized = false;
    staysOnTop = false;

    hasUrlEdit = true;
    hasHomeButton = true;
    hasBackButton = true;
    hasReloadButton = true;
    hasForwardButton = true;
    hasStopButton = true;

    certID = "12345CABLELABS67890";
    tvRemoteURL = "http://localhost/remote.html";

    defaultRUIUrl = "http://ostevetto.com";
    defaultRUIImage = "http://ostevetto.com/images/BassClef.ico";
    defaultRUILabel = "Live Jazz";
}

void BrowserSettings::save()
{
    setValue(keyTitleBar, hasTitleBar );
    setValue(keyMenuBar, hasMenuBar);
    setValue(keyNavigationBar, hasNavigationBar);
    setValue(keyTVRemote, hasTVRemote);
    setValue(keyStartMaximized, startMaximized);
    setValue(keyStaysOnTop, staysOnTop);

    setValue(keyUrlEdit, hasUrlEdit);
    setValue(keyHomeButton, hasHomeButton);
    setValue(keyBackButton, hasBackButton);
    setValue(keyReloadButton, hasReloadButton);
    setValue(keyForwardButton, hasForwardButton);
    setValue(keyStopButton, hasStopButton);

    setValue(keyCertID, certID);
    setValue(keyTVRemoteUrl, tvRemoteURL);

    setValue(keyRUIUrl, defaultRUIUrl);
    setValue(keyRUIImage, defaultRUIImage);
    setValue(keyRUILabel, defaultRUILabel);
}

BrowserSettings* BrowserSettings::Instance()
{
    if ( !m_pInstance )
    {
        m_pInstance = new BrowserSettings;
    }

    return m_pInstance;
}
