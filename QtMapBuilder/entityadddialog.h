#ifndef ENTITYADDDIALOG_H
#define ENTITYADDDIALOG_H

#include <QDialog>

namespace Ui {
class EntityAddDialog;
}
#include "entity.h"
class EntityAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EntityAddDialog(QWidget *parent = 0);
    ~EntityAddDialog();

    void setEntity(Entity &ent);
private slots:
    void updateEntity();

protected:
    void setEnemy();
    void setItem();
    void setGold();
    Entity* mEnt;
private:
    Ui::EntityAddDialog *ui;
};

#endif // ENTITYADDDIALOG_H
