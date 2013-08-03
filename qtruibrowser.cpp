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
#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QSettings>
#include <QTextStream>


static void printUsage(const QString& program)
{
    QTextStream(stderr) << "Usage: " << program << " [-h | --help] [--fullscreen] [url]" << endl;
}

static void applyDefaultSettings()
{

    QWebSettings::setMaximumPagesInCache(4);

    QWebSettings::setObjectCacheCapacities((16*1024*1024) / 8, (16*1024*1024) / 8, 16*1024*1024);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    QWebSettings::enablePersistentStorage();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    const QStringList& args = app.arguments();
    bool startFullScreen = false;
    QString uri;
    for (int i = 1; i < args.size(); ++i) {
        const QString& arg = args[i];
        if (arg == "--fullscreen") {
            startFullScreen = true;
        } else if (arg == "--help" || arg == "-h") {
            printUsage(args[0]);
            return 1;
        } else if (uri.isEmpty() && !arg.startsWith('-') && !arg.startsWith("--")) {
            uri = arg;
        } else {
            QTextStream(stderr) << "Error: Unknown argument " << arg << endl;
            printUsage(args[0]);
            return 1;\
        }
    }
    applyDefaultSettings();

    app.setOrganizationName("CableLabs");
    app.setApplicationName("QtRUIBrowser");
    app.setApplicationVersion("0.1");

    MainWindow window(startFullScreen);

    if (!uri.isEmpty()) {
        window.load(uri);
    } else {
        window.home();
    }
    window.show();
    window.checkHttpProxyEnabled();

    return app.exec();
}
