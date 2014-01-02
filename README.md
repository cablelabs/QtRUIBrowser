# QT RUI Browser README

These instructions assume you've build Qt5 and WebKit, following our standard
instructions.

## Build

    # Your path should be setup already from building WebKit, like so:
    export QTDIR=~/workspace/qt5/qtbase
    export PATH=$QTDIR/bin:$PATH

    # This should point to your WebKit checkout
    export WEBKIT_ROOT=~/workspace/webkit

    # This should point to your OpenSSL build which provides TLS extension and TLS supplemental data support
    export OPENSSL_ROOT=/usr/local/ssl

    # This makes a debug version
    qmake QtRUIBrowser.pro
    
    # This makes a release version
    qmake QtRUIBrowser.pro CONFIG+=release CONFIG-=debug

    make

## Run

    # Make sure this points to wherever you built WebKit
    # The binary will be in either bin/debug or bin/release based on how you built
    # Change DTCP_LIBRARY_PATH and DTCP_KEY_STORAGE_DIR to reference the fully qualified path to the DTCP wrapper library and the DTCP key directory, respectively
    LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/webkit/WebKitBuild/Debug/lib DTCP_LIBRARY_PATH=/media/truecrypt1/dtcpmods/dtcpip_cvp2_test.so DTCP_KEY_STORAGE_DIR=/media/truecrypt1/dtcp-ip_sik /bin/debug/QtRUIBrowser

## Operation / Testing

Test using the CableLabs RUIServer (java mock RUI Servers, see the related README)

Select a RUI by:

  * Clicking on the UI entry.
  * Entering the number via the keyboard.
  * Scrolling up/down until the desired UI is highlighted, then pressing enter.

Return to the Navigation Page via ctrl/escape key or back button.

NOTE: This was changed from just escape because escape is used to exit full screen mode.
