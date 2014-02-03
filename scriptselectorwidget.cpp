#include "scriptselectorwidget.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QStyle>
#include <QComboBox>
#include <QFileDialog>
ScriptSelectorWidget::ScriptSelectorWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::Popup);

    createLayout();
}

void ScriptSelectorWidget::open()
{
    emit(openClicked());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Abrir script lua"),
                                                    QString(),
                                                    tr("Lua script (*.lua)"));
    if(fileName.isEmpty() == false) {
        scriptPath = fileName;
        scriptNameCombo->clear();
        QString baseName = QFileInfo(scriptPath).baseName();
        scriptNameCombo->addItem(baseName);
    }
}

void ScriptSelectorWidget::runScript()
{
    emit(runScriptClicked(scriptPath));
}

void ScriptSelectorWidget::createLayout()
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    scriptNameCombo = new QComboBox(this);
    scriptNameCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    scriptNameCombo->setMaximumWidth(300);
    layout->addWidget(scriptNameCombo);

    QSize iconSize(20,20);

    QToolButton* openButton = new QToolButton(this);
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    openButton->setIconSize(iconSize);
    openButton->setToolTip(tr("Abrir pasta"));
    connect(openButton, SIGNAL(clicked()),this, SLOT(open()));
    layout->addWidget(openButton);

    QToolButton* runButton = new QToolButton(this);
    runButton->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
    runButton->setIconSize(iconSize);
    runButton->setToolTip(tr("Executar script"));
    connect(runButton, SIGNAL(clicked()),this, SLOT(runScript()));
    layout->addWidget(runButton);

    setLayout(layout);
}
