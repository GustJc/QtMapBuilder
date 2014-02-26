#ifndef SCRIPTSELECTORWIDGET_H
#define SCRIPTSELECTORWIDGET_H

#include <QWidget>
class QComboBox;
class ScriptSelectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptSelectorWidget(QWidget *parent = 0);

signals:
    void openClicked();
    void runScriptClicked(QString filename);
public slots:
    void open();
    void runScript();
private:
    void createLayout();
    QComboBox* scriptNameCombo;
    QString scriptPath;

};

#endif // SCRIPTSELECTORWIDGET_H
