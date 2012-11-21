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
    $${ROOT_WEBKIT_DIR}/Source/WTF

SOURCES += \
    locationedit.cpp \
    qtruibrowser.cpp \
    mainwindow.cpp \
    utils.cpp \
    browsersettings.cpp \
    ruiwebpage.cpp \
    discoveryproxy.cpp \
    soapmessage.cpp \
    userinterface.cpp \
    userinterfacemap.cpp

HEADERS += \
    locationedit.h \
    mainwindow.h \
    utils.h \
    browsersettings.h \
    ruiwebpage.h \
    discoveryproxy.h \
    soapmessage.h \
    userinterface.h \
    userinterfacemap.h \
    webinspector.h


WEBKIT += wtf webcore webkit1

CONFIG -= production_build
WEBKIT_CONFIG -= build_webkit1
WEBKIT_CONFIG -= build_webkit2

CONFIG(debug, debug|release) {
    DESTDIR = $(PWD)/bin/debug
} else {
    DESTDIR = $(PWD)/bin/release
}
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
    www/rui.js \
    www/rui.css \
    www/rui_elementFrame.png \
    www/rui_elementNumber.png \
    www/rui_elementName.png \
    www/rui_arrowUp.png \
    www/rui_arrowDown.png \
    README \
    www/rui_missingIcon.png \
    www/rui_elementNumber2.png \
    www/rui_elementName2.png \
    www/rui_background2.png \
    www/rui_elementNumberHighlight.png \
    www/rui_elementNameHighlight.png \
    www/rui_background.jpg
