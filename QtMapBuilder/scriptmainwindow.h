#ifndef SCRIPTMAINWINDOW_H
#define SCRIPTMAINWINDOW_H

#include <QMainWindow>
class QTreeWidget;
class QsciScintilla;
class QAction;
class QToolbar;
class ScriptMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ScriptMainWindow(QWidget *parent = 0);

    void openFile(const QString filename);
    void setCurrentFile(const QString filename);

    void initializeActions();
    void initializeUI();
    void initializeEditor();
    void initializeLexer();
public slots:
    void onOpen();
    void onSaveFile();
protected slots:
    void onTextChanged();
    void onExecuteScript();
    void onTestScript();
private:
    void displayCodeErrors(const QStringList strList);
protected:
    QsciScintilla* editor;
    QTreeWidget*   treeSelector;
    QString        curFile;

    QAction*       executeScriptAction;
    QAction*       testScriptAction;
    QAction*       saveAction;
    QToolBar*      btnToolBar;

};

#endif // SCRIPTMAINWINDOW_H
