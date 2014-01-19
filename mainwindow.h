#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QAction;
class QMenu;
class QDockWidget;
class TilesetEditor;
class QImage;
class MapView;
class QGraphicsView;
class QTreeWidget;
class QTreeWidgetItem;
#include <QVector>
#include "entity.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QDockWidget* tilesetDock;
    TilesetEditor* tilesetEditor;
    QTreeWidget* entitySelector;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void mapWasModified();
    void updateStatusBar(const QString &text );
    void entityItemChanged();

    void tooglePaintTool(bool isChecked);
    void toogleCursorTool(bool isChecked);
    void toogleRectangleTool(bool isChecked);
    void toogleEditMode(bool isChecked);
protected:
    void closeEvent(QCloseEvent *event);

    QImage* tilesetImage;
    QImage* enemyImage;
    QImage* itemImage;

    MapView* mapView;

private:
    void populateTreeList();
    void connectObjects();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void writeSettings();
    void saveSettings();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
        QMenu *viewFerramentasSubMenu;
    QMenu *helpMenu;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    //View/Mode
    QAction *editModeAction;
    QAction *showPathAction;
    QAction *showGridAction;
    //Menu
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *aboutAction;
    //Tool
    QAction *paintToolAction;
    QAction *rectangleToolAction;
    QAction *cursorToolAction;

private:
    //Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
