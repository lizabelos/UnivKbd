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

#ifndef VirtualKeyboardConfigurationWidget_H
#define VirtualKeyboardConfigurationWidget_H

#include <QWidget>
#include <QPointer>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QListView>
#include <QStringListModel>

namespace UniQKey {

    /**
     * @class VirtualKeyboardConfigurationWidget
     *
     * @brief Represents a virtual keyboard configuration widget.
     *
     * The VirtualKeyboardConfigurationWidget class provides a customizable widget that can be used
     * to configure the virtual keyboard.
     *
     * You don't have to instantiate this class yourself, as it is automatically created by the VirtualKeyboard class.
     *
     * @see VirtualKeyboard
     */
    class VirtualKeyboardConfigurationWidget : public QTabWidget {
    Q_OBJECT

    public:
        VirtualKeyboardConfigurationWidget(QWidget *parent = nullptr);

    signals:
        void requestKeyboard(QString country, QString layout);

        void close();

    protected slots:
        void onTabChanged(int index);

    private:
        QPointer<QWidget> mLanguagesTab;
        QPointer<QHBoxLayout> mLanguagesTabLayout;
        QPointer<QListView> mCountrySelector;
        QPointer<QListView> mLayoutSelector;
        QPointer<QStringListModel> mCountrySelectorModel;
        QPointer<QStringListModel> mLayoutSelectorModel;


    };

}

#endif // VirtualKeyboardConfigurationWidget_H