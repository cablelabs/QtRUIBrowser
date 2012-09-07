/*
 * (c) 2012 Cable Television Laboratories, Inc. All rights reserved. Proprietary and Confidential.
 *
 * tvremotebridge.cpp
 * QtRUIBrowser
 *
 * Created by: sjohnson on 9/6/2012.
 *
 * Description: JavaScript bridge for the TV Remote page to marshal key presses to the navigation page
 *
 */

#include "tvremotebridge.h"
#include "mainwindow.h"
#include "stdlib.h"
#include "stdio.h"
#include <QWebFrame>
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QWebElement>

TVRemoteBridge::TVRemoteBridge( MainWindow* mainWindow, QWebPage* pageMain, QWebPage* pageRemote )
    :m_mainWindow(mainWindow), m_pageMain(pageMain), m_pageRemote(pageRemote)
{
    // Initial attach object
    attachObject();

    // Callback to reattach when new page is loaded
    connect( m_pageRemote->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachObject()) );
}

// Give javascript access to our public slots
void TVRemoteBridge::attachObject()
{
    m_pageRemote->mainFrame()->addToJavaScriptWindowObject( QString("TVRemoteBridge"), this );
}

// The only exposed method - recieve notifications when a button on the tv remote simulator is pressed
void TVRemoteBridge::remoteButtonPressed( const QString &param )
{
    fprintf( stderr, "TVRemoteBridge: %s\n", param.toAscii().data());

    int keyCode = Qt::Key_Question;

    if ( !param.compare("b0")) keyCode = Qt::Key_0;
    else if ( !param.compare("b1")) keyCode = Qt::Key_1;
    else if ( !param.compare("b2")) keyCode = Qt::Key_2;
    else if ( !param.compare("b3")) keyCode = Qt::Key_3;
    else if ( !param.compare("b4")) keyCode = Qt::Key_4;
    else if ( !param.compare("b5")) keyCode = Qt::Key_5;
    else if ( !param.compare("b6")) keyCode = Qt::Key_6;
    else if ( !param.compare("b7")) keyCode = Qt::Key_7;
    else if ( !param.compare("b8")) keyCode = Qt::Key_8;
    else if ( !param.compare("b9")) keyCode = Qt::Key_9;
    //else if ( !param.compare("la")) keyCode = Qt::Key_Left;
    //else if ( !param.compare("ra")) keyCode = Qt::Key_Right;
    //else if ( !param.compare("ua")) keyCode = Qt::Key_Up;
    //else if ( !param.compare("da")) keyCode = Qt::Key_Down;
    else if ( !param.compare("la")) keyCode = Qt::Key_L;    // Temp for testing
    else if ( !param.compare("ra")) keyCode = Qt::Key_R;    // Temp for testing
    else if ( !param.compare("ua")) keyCode = Qt::Key_U;    // Temp for testing
    else if ( !param.compare("da")) keyCode = Qt::Key_D;    // Temp for testing
    else if ( !param.compare("select")) keyCode = Qt::Key_S;
    else if ( !param.compare("play")) keyCode = Qt::Key_P;
    else if ( !param.compare("pause")) keyCode = Qt::Key_A;
    else if ( !param.compare("menu")) keyCode = Qt::Key_M;

    QKeyEvent key(QEvent::KeyPress, keyCode, Qt::NoModifier);
    QApplication::sendEvent(m_pageMain, &key);

    // Temp for testing - use pause button to toggle frame.
    if (keyCode == Qt::Key_A) {
        Qt::WindowFlags flags = m_mainWindow->windowFlags();
        if ( flags & Qt::FramelessWindowHint )
            flags &= ~Qt::FramelessWindowHint;
        else
            flags |= Qt::FramelessWindowHint;

        m_mainWindow->setWindowFlags(flags);
        m_mainWindow->show();

    } else if (keyCode == Qt::Key_S) {

        // Experiment with adding div
        /*
            QWebElement existingElement;
            existingElement.appendInside( '<div/>' );
            QWebElement newElement = existingElement.lastChild();

         */
        QWebFrame *frame = m_pageMain->mainFrame();
        QWebElement document = frame->documentElement();
        document.appendInside( "<h2>Oh yes I DID!</h2>");
    }
}

