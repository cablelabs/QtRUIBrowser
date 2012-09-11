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
    soapmessage.cpp

HEADERS += \
    locationedit.h \
    mainwindow.h \
    utils.h \
    tvremotebridge.h \
    browsersettings.h \
    ruiwebpage.h \
    discoveryproxy.h \
    discoverystub.h \
    soapmessage.h


WEBKIT += wtf webcore

DESTDIR = bin
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
