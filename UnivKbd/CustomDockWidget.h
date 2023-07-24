/*
* --------------------------------------------------------------
* Project: UnivKbd
* Author: Liza Belos
* Year: 2023
*
* Copyright (c) 2023. All rights reserved.
* This work is licensed under the terms of the MIT License.
* For a copy, see <https://opensource.org/licenses/MIT>.
* --------------------------------------------------------------
*
* NOTICE:
* This file is part of the original distribution of the UnivKbd project.
* All changes and redistributions of this file must retain this notice,
* the list of contributors, and the entire copyright notice including the
* MIT License information.
*
* DISCLAIMER:
* This software is provided 'as-is', without any express or implied warranty.
* In no event will the authors be held liable for any damages arising from
* the use of this software.
*/

#ifndef CustomDockWidget_H
#define CustomDockWidget_H

#include <QWidget>
#include <QPointer>
#include <QDockWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QStyle>

namespace UnivKbd {

    /**
     * @class CustomDockWidget
     *
     * @brief A custom dock widget with a wide moving and resizing area.
     */
    class CustomDockWidget : public QDockWidget
    {
    Q_OBJECT

    public:
        explicit CustomDockWidget(QWidget *parent = nullptr) : QDockWidget(parent, Qt::Window | Qt::CustomizeWindowHint) {
            // disable the title bar
            setTitleBarWidget(new QWidget(this));

        }

        void setCustomWidget(QWidget *widget) {
            mContentWidget = new QWidget(this);
            mContentLayout = new QVBoxLayout;
            mContentLayout->setContentsMargins(0, 40, 0, 0); // Provide enough space for the title bar.
            mContentWidget->setLayout(mContentLayout);
            setWidget(mContentWidget);
            mContentLayout->addWidget(widget);
        }

    protected:
        void mousePressEvent(QMouseEvent *event) override {
            // start dragging if mouse pressed on the title bar
            if (event->button() == Qt::LeftButton && titleBarRect().contains(event->pos())) {
                mDragStartPosition = event->pos();
                mDrag = true;
                if (!isFloating()) {
                    setFloating(true);
                }
            }

            if (event->button() == Qt::RightButton) {
                setFloating(false);
            }

            // start resizing if mouse pressed on the resize handle
            if (event->button() == Qt::LeftButton && resizeHandleRect().contains(event->pos())) {
                mResizeStartPosition = event->globalPos();
                mResize = true;
            }
        }

        void mouseMoveEvent(QMouseEvent *event) override {
            // move the widget
            if (mDrag) {
                QPoint diff = event->pos() - mDragStartPosition;
                move(pos() + diff);
            }

            // resize the widget
            if (mResize) {
                QPoint diff = mResizeStartPosition - event->globalPos();
                QSize newSize = mContentWidget->size() + QSize(diff.x(), diff.y());
                mContentWidget->setFixedHeight(newSize.height());
                mContentWidget->setFixedWidth(newSize.width());
                mResizeStartPosition = event->globalPos();

                // also move the widget if it is floating, to follow the mouse cursor
                if (isFloating()) {
                    move(pos() - diff);
                }
            }
        }

        void mouseReleaseEvent(QMouseEvent *event) override {
            (void) event;
            mDrag = false;
            mResize = false;
        }

        // two finger touch events
        /*void touchEvent(QTouchEvent *event) override {
            if (event->touchPoints().size() == 2) {
                if (event->touchPointStates() & Qt::TouchPointPressed) {
                    setFloating(false);
                }
            }
        }*/
        // use event() to support qt5
        bool event(QEvent *event) override {
            if (event->type() == QEvent::TouchBegin && static_cast<QTouchEvent *>(event)->touchPoints().size() == 2) {
                setFloating(false);
            }
            return QDockWidget::event(event);
        }

        void paintEvent(QPaintEvent *) override {
            QPainter painter(this);

            // draw title bar
            QRect titleBar = titleBarRect();
            painter.fillRect(titleBar, Qt::lightGray);

            // draw resize handle
            QRect resizeHandle = resizeHandleRect();
            painter.drawText(resizeHandle, Qt::AlignCenter, "⤢");
        }

    private:
        QPointer<QWidget> mContentWidget;
        QPointer<QVBoxLayout> mContentLayout;

        QPoint mDragStartPosition;
        QPoint mResizeStartPosition;
        bool mDrag = false;
        bool mResize = false;

        QRect titleBarRect() const {
            return QRect(resizeHandleRect().width(), 0, QWidget::width() - resizeHandleRect().width(), 40);
        }

        QRect resizeHandleRect() const {
            return QRect(0, 0, QWidget::width() / 2, 40);
        }

    };

}  // namespace UnivKbd


#endif // CustomDockWidget_H