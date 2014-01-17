#ifndef ENTITYADDDIALOG_H
#define ENTITYADDDIALOG_H

#include <QDialog>

namespace Ui {
class EntityAddDialog;
}

class EntityAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EntityAddDialog(QWidget *parent = 0);
    ~EntityAddDialog();

private:
    Ui::EntityAddDialog *ui;
};

#endif // ENTITYADDDIALOG_H
