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
    bool hasTVRemote;
    bool hasHomeButton;
    bool hasBackButton;
    bool hasForwardButton;
    bool hasReloadButton;
    bool hasStopButton;
    bool startMaximized;
    bool staysOnTop;
    QString certID;
    QString defaultRUIUrl;
    QString defaultRUIImage;
    QString defaultRUILabel;
    QString tvRemoteURL;
    void save();

};

#endif // BROWSERSETTINGS_H
