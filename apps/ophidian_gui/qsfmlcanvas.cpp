/*
 * Copyright (c) 2013 GNKW and SFML code parts
 *
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */
#include "qsfmlcanvas.h"

// Platform-specific headers
#ifdef Q_WS_X11
    #include <Qt/qx11info_x11.h>
    #include <X11/Xlib.h>
#endif

using namespace qsfml;

QSFMLCanvas::QSFMLCanvas(QWidget* parent, const QPoint& position, const QSize& size, unsigned int frameTime) :
QWidget       (parent),
myInitialized (false)
{
    // Setup some states to allow direct rendering into the widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // Strong focus for events in SFML
    setFocusPolicy(Qt::StrongFocus);

    // Setup the widget geometry
    //move(position);
    //resize(size);

    // Add Vertical sync to SFML
    //this->setVerticalSyncEnabled(true);
    //this->setFramerateLimit(60);

    // Setup the timer
    myTimer.setInterval(frameTime);
}

QSFMLCanvas::~QSFMLCanvas()
{
    // Nothing to do...
}

void QSFMLCanvas::OnInit()
{
    // Nothing to do by default...
}

void QSFMLCanvas::OnUpdate()
{
    // Nothing to do by default...
}


void QSFMLCanvas::showEvent(QShowEvent*)
{
    if (!myInitialized)
    {
        #ifdef Q_WS_X11
           XFlush(QX11Info::display());
       #endif
        // Create the SFML window with the widget handle
        sf::Window::create(winId());

        // Let the derived class do its specific stuff
        OnInit();

        // Setup the timer to trigger a refresh at specified framerate
        connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
        myTimer.start();

        myInitialized = true;
    }
}

void QSFMLCanvas::paintEvent(QPaintEvent*)
{
    // Let the derived class do its specific stuff
    OnUpdate();

    // Display on SFML widget
    sf::Window::display();
}

QPaintEngine* QSFMLCanvas::paintEngine() const
{
    // Don't use Qt render engine
    return 0;
}
