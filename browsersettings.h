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
#ifndef BROWSERSETTINGS_H
#define BROWSERSETTINGS_H

#include <QSettings>

class BrowserSettings : public QSettings
{
    Q_OBJECT

public:
    explicit BrowserSettings(QObject *parent = 0);
    static BrowserSettings* Instance();

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
