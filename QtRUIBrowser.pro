# -------------------------------------------------------------------
# Project file for the QtRUIBrowser binary
#
# See 'Tools/qmake/README' for an overview of the build system
# -------------------------------------------------------------------

TEMPLATE = app

# Define WEBKIT_ROOT environment variable external to this script - Do not use relative paths or ~/
# 

INCLUDEPATH += \
    $(WEBKIT_ROOT)/Source/WebCore/Modules/discovery \
    $(WEBKIT_ROOT)/Source/WebCore/platform/qt \
    $(WEBKIT_ROOT)/Source/WebKit/qt/WebCoreSupport \
    $(WEBKIT_ROOT)/Source/WTF

CONFIG(debug, debug|release) {
    INCLUDEPATH += $(WEBKIT_ROOT)/WebKitBuild/Debug/include/QtWebKitWidgets
} else {
    INCLUDEPATH += $(WEBKIT_ROOT)/WebKitBuild/Release/include/QtWebKitWidgets
}

QMAKE_CXXFLAGS += $$(CXXFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_LFLAGS += $$(LDFLAGS)

DEFINES += WTF_PLATFORM_QT=1

SOURCES += \
    browsersettings.cpp \
    discoveryproxy.cpp \
    locationedit.cpp \
    mainwindow.cpp \
    qtruibrowser.cpp \
    ruiwebpage.cpp \
    soapmessage.cpp \
    userinterface.cpp \
    userinterfacemap.cpp \
    utils.cpp

HEADERS += \
    browsersettings.h \
    discoveryproxy.h \
    locationedit.h \
    mainwindow.h \
    ruiwebpage.h \
    soapmessage.h \
    userinterface.h \
    userinterfacemap.h \
    utils.h \
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

QT += network webkit widgets webkitwidgets xml

macx:QT += xml

contains(DEFINES, HAVE_FONTCONFIG=1): PKGCONFIG += fontconfig

contains(QT_CONFIG, opengl) {
    QT += opengl
    DEFINES += QT_CONFIGURED_WITH_OPENGL
}

RESOURCES += \
    QtRUIBrowser.qrc

OTHER_FILES += \
    README \
    www/index.html \
    www/jquery.1.5.0.min.js \
    www/jquery-ui.1.8.9.min.js \
    www/rui.js \
    www/rui.css \
    www/rui_arrowDown.png \
    www/rui_arrowUp.png \
    www/rui_background.jpg \
    www/rui_background2.png \
    www/rui_background_demo.png \
    www/rui_background_dlna.png \
    www/rui_elementFrame.png \
    www/rui_elementName.png \
    www/rui_elementName2.png \
    www/rui_elementNameHighlight.png \
    www/rui_elementNumber.png \
    www/rui_elementNumber2.png \
    www/rui_elementNumberHighlight.png \
    www/rui_missingIcon.png \
    www/rui_titleWipe.png
