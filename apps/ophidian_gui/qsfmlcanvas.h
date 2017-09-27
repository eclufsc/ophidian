/*
 * Copyright (c) 2013 GNKW and SFML code parts
 *
 * This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */
#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>
#include <QWidget>
#include <QPoint>
#include <QSize>
#include <QTimer>
#include <QPaintEngine>
#include <QShowEvent>
#include <QPaintEvent>

namespace qsfml {
    /**
     * @brief Canvas SFML for Qt
     * Inspiration from SFML Website and some code parts
     * @author Anthony Rey <anthony.rey@mailoo.org>
     */
    class QSFMLCanvas : public QWidget, public sf::RenderWindow
    {
    private :
        /**
         * @brief Timer used to update the view
         */
        QTimer myTimer;
        /**
         * @brief Tell whether the SFML window has been initialized or not
         */
        bool myInitialized;
    public :

        /**
         * Construct the QSFMLCanvas
         * @param parent Parent of the widget
         * @param position Position of the widget
         * @param size Size of the widget
         * @param frameTime Frame duration, in milliseconds (0 by default)
         */
        QSFMLCanvas(QWidget* parent, const QPoint& position, const QSize& size, unsigned int frameTime = 0);

        /**
        * Destructor
        */
        virtual ~QSFMLCanvas();

    private :

        /**
         * @brief Initialisation of graphics
         */
        virtual void OnInit();

        /**
         * @brief Update of graphics
         */
        virtual void OnUpdate();

        /**
         * @brief It's used to don't use Qt render engine
         * @return The paint engine
         */
        virtual QPaintEngine* paintEngine() const;

        /**
        * @brief Called when the widget is shown, we use it to initialize our SFML window
        */
        virtual void showEvent(QShowEvent*);

        /**
        * @brief Called when the widget needs to be painted, we use it to display a new frame
        */
        virtual void paintEvent(QPaintEvent*);
    };
}


#endif // QSFMLCANVAS_H
