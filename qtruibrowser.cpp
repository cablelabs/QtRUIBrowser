/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * qtruibrowser.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: Application main for QtRUIBrowser
 *
 */

#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QSettings>


void applyDefaultSettings()
{

    QWebSettings::setMaximumPagesInCache(4);

    QWebSettings::setObjectCacheCapacities((16*1024*1024) / 8, (16*1024*1024) / 8, 16*1024*1024);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);


    // Other settings (most of these were command line args in QtTestBrowser)
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::FrameFlatteningEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::WebGLEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::ZoomTextOnly, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::WebGLEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::SpatialNavigationEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::FrameFlatteningEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    //QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);

    QWebSettings::enablePersistentStorage();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    applyDefaultSettings();

    app.setOrganizationName("CableLabs");
    app.setApplicationName("QtRUIBrowser");
    app.setApplicationVersion("0.1");

    MainWindow* window = new MainWindow();
    window->home();

    window->show();
    return app.exec();
}


//#include "qtruibrowser.moc"
