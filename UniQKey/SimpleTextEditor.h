/*
* --------------------------------------------------------------
* Project: UniQKey
* Author: Liza Belos
* Year: 2023
* 
* Copyright (c) 2023. All rights reserved.
* This work is licensed under the terms of the MIT License.
* For a copy, see <https://opensource.org/licenses/MIT>.
* --------------------------------------------------------------
*
* NOTICE:
* This file is part of the original distribution of the UniQKey project. 
* All changes and redistributions of this file must retain this notice, 
* the list of contributors, and the entire copyright notice including the
* MIT License information.
* 
* DISCLAIMER:
* This software is provided 'as-is', without any express or implied warranty. 
* In no event will the authors be held liable for any damages arising from 
* the use of this software.
*/


#ifndef SIMPLETEXTEDITOR_H
#define SIMPLETEXTEDITOR_H

#include <QApplication>
#include <QLabel>
#include <QTextEdit>
#include <QMainWindow>
#include <QPointer>
#include <QAction>
#include <QTextDocument>
#include <QGridLayout>
#include <QSpacerItem>
#include <QMenuBar>
#include <QToolBar>
#include <QTextCharFormat>
#include <QStatusBar>
#include <QSlider>
#include <QWheelEvent>
#include <QScrollArea>
#include <QLineEdit>


namespace UniQKey {

    class SimpleTextEditor : public QMainWindow {
    Q_OBJECT

    public:
        SimpleTextEditor(QWidget *parent = 0);

    protected:
        void wheelEvent(QWheelEvent *event) override;

    private slots:

        void makeTextBold();

        void makeTextItalic();

        void makeTextUnderlined();

        void updateFormatActions(const QTextCharFormat &format);

        void zoom(int level);

        void zoomIn();

        void zoomOut();

    private:
        QPointer<QTextEdit> mTextEditor;
        QPointer<QLineEdit> mSearchLine;
        QPointer<QTextDocument> mTextDocument;

        QPointer<QAction> boldAction;
        QPointer<QAction> italicAction;
        QPointer<QAction> underlineAction;

        qreal mZoomFactor = 1.0;

    };

}


#endif // SIMPLETEXTEDITOR_H