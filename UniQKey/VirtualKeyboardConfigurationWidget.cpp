#include "VirtualKeyboardConfigurationWidget.h"
#include "Keyboard.h"

#include <QScroller>

UniQKey::VirtualKeyboardConfigurationWidget::VirtualKeyboardConfigurationWidget(QWidget *parent) : QTabWidget(parent) {
    mLanguagesTab = new QWidget(this);
    mLanguagesTabLayout = new QHBoxLayout();
    mCountrySelector = new QListView();
    mLayoutSelector = new QListView();

    mLanguagesTabLayout->addWidget(mCountrySelector);
    mLanguagesTabLayout->addWidget(mLayoutSelector);
    mLanguagesTab->setLayout(mLanguagesTabLayout);

    addTab(mLanguagesTab, "Languages");
    addTab(new QWidget(this), "Close");

    connect(this, &QTabWidget::currentChanged, this, &VirtualKeyboardConfigurationWidget::onTabChanged);

    QList<QString> keyboardLayouts = Keyboard::listExportedKeyboards();
    if (keyboardLayouts.size() == 0) {
        throw std::runtime_error("No keyboard layouts found");
    }

    // add a model to select the keyboard layout
    mCountrySelectorModel = new QStringListModel(keyboardLayouts);
    mCountrySelector->setModel(mCountrySelectorModel);
    // set to English (United States) as default
    mCountrySelector->setCurrentIndex(mCountrySelectorModel->index(keyboardLayouts.indexOf("US")));


    QList<QString> keyboardLayoutsAll = getKeyboardLayouts();
    mLayoutSelectorModel = new QStringListModel(keyboardLayoutsAll);
    mLayoutSelector->setModel(mLayoutSelectorModel);
    mLayoutSelector->setCurrentIndex(mLayoutSelectorModel->index(0));

    connect(mCountrySelector->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &index) {
        emit requestKeyboard(index.data().toString(), mLayoutSelector->currentIndex().data().toString());
    });

    connect(mLayoutSelector->selectionModel(), &QItemSelectionModel::currentChanged, [=](const QModelIndex &index) {
        emit requestKeyboard(mCountrySelector->currentIndex().data().toString(), index.data().toString());
    });

    // one finger scrolling on the list views
    QScroller::grabGesture(mCountrySelector, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(mLayoutSelector, QScroller::LeftMouseButtonGesture);

    // swipe gesture to change tabs
    QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);

}

void UniQKey::VirtualKeyboardConfigurationWidget::onTabChanged(int index) {
    if (this->tabText(index) == "Close") {
        emit close();
        setCurrentIndex(0);
    }
}
