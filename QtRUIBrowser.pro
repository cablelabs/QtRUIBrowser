# -------------------------------------------------------------------
# Project file for the QtRUIBrowser binary
#
# See 'Tools/qmake/README' for an overview of the build system
# -------------------------------------------------------------------

TEMPLATE = app

# Define WEBKITDIR environment variable external to this script - Do not use relative paths or ~/
ROOT_WEBKIT_DIR = $$(WEBKITDIR)

INCLUDEPATH += \
    $${ROOT_WEBKIT_DIR}/Source/WebCore/Modules/discovery \
#    $${ROOT_WEBKIT_DIR}/Source/WebKit/qt/WebCoreSupport \
#    $${ROOT_WEBKIT_DIR}/Source/WTF

SOURCES += \
    locationedit.cpp \
    qtruibrowser.cpp \
    mainwindow.cpp \
    utils.cpp \
    tvremotebridge.cpp \
    browsersettings.cpp \
    ruiwebpage.cpp \
    discoveryproxy.cpp \
    discoverystub.cpp \
    soapmessage.cpp \
    userinterface.cpp \
    userinterfacemap.cpp

HEADERS += \
    locationedit.h \
    mainwindow.h \
    utils.h \
    tvremotebridge.h \
    browsersettings.h \
    ruiwebpage.h \
    discoveryproxy.h \
    discoverystub.h \
    soapmessage.h \
    userinterface.h \
    userinterfacemap.h


WEBKIT += wtf webcore webkit1

#CONFIG += debug
#CONFIG -= release
CONFIG -= production_build
WEBKIT_CONFIG -= build_webkit2

DESTDIR = $(PWD)/bin
OBJECTS_DIR = obj

QT += network webkit widgets xml

macx:QT += xml

contains(DEFINES, HAVE_FONTCONFIG=1): PKGCONFIG += fontconfig

contains(QT_CONFIG, opengl) {
    QT += opengl
    DEFINES += QT_CONFIGURED_WITH_OPENGL
}

RESOURCES += \
    QtRUIBrowser.qrc

OTHER_FILES += \
    www/index.html \
    www/rui_background.png \
    www/rui.js \
    www/rui.css \
    www/rui_elementFrame.png \
    www/rui_elementNumber.png \
    www/rui_elementName.png \
    www/rui_arrowUp.png \
    www/rui_arrowDown.png
