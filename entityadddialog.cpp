#include "entityadddialog.h"
#include "ui_entityadddialog.h"

EntityAddDialog::EntityAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EntityAddDialog)
{
    ui->setupUi(this);
}

EntityAddDialog::~EntityAddDialog()
{
    delete ui;
}
