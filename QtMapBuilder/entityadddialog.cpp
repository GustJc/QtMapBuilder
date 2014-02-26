#include "entityadddialog.h"
#include "ui_entityadddialog.h"
#include "entity.h"
#include <qdebug.h>
EntityAddDialog::EntityAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntityAddDialog)
{
    ui->setupUi(this);
    QObject::connect(ui->buttonBox, SIGNAL(accepted()),this,
                                           SLOT(updateEntity()));
}

EntityAddDialog::~EntityAddDialog()
{
    delete ui;
}

void EntityAddDialog::setEntity(Entity &ent)
{
    mEnt = &ent;
    if(ent.typeId() == ENTITY_ENEMY) setEnemy();
    else if(ent.typeId() == ENTITY_ITEM) setItem();
    else if(ent.typeId() == ENTITY_GOLD) setGold();
}

void EntityAddDialog::updateEntity()
{
    qDebug() << "UpdateEntity";
    mEnt->mAtk = ui->ataqueSpinBox->value();
    mEnt->mDef = ui->defesaSpinBox->value();
    mEnt->mDelay = ui->velocidadeSpinBox->value();
    mEnt->mRange = ui->visibilidadeSpinBox->value();
    mEnt->mHp = ui->hpSpinBox->value();
    mEnt->mMp = ui->mpSpinBox->value();
    mEnt->mName = ui->nomeLineEdit->text();
}

void EntityAddDialog::setEnemy()
{
    ui->nomeLineEdit->setText(mEnt->mName);
    ui->ataqueSpinBox->setValue(mEnt->mAtk);
    ui->defesaSpinBox->setValue(mEnt->mDef);
    ui->velocidadeSpinBox->setValue(mEnt->mDelay);
    ui->visibilidadeSpinBox->setValue(mEnt->mRange);
    ui->hpSpinBox->setValue(mEnt->mHp);
    ui->mpSpinBox->setValue(mEnt->mMp);
}

void EntityAddDialog::setItem()
{
    ui->nomeLineEdit->setText(mEnt->mName);
    ui->ataqueSpinBox->setValue(mEnt->mAtk);
    ui->defesaSpinBox->setValue(mEnt->mDef);
    ui->hpSpinBox->setValue(mEnt->mHp);
    ui->mpSpinBox->setValue(mEnt->mMp);


    ui->velocidadeSpinBox->setVisible(false);
    ui->visibilidadeSpinBox->setVisible(false);
    ui->velocidadeLabel->setVisible(false);
    ui->visibilidadeLabel->setVisible(false);
}

void EntityAddDialog::setGold()
{

}
