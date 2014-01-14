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

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void mapWasModified();
    void updateStatusBar(const QString &text );
protected:
    void closeEvent(QCloseEvent *event);

    QImage* tilesetImage;
    MapView* mapView;
private:
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
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *aboutAction;

private:
    //Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
