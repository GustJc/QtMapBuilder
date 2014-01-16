#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QtGui>
#include "tileseteditor.h"
#include "mapview.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)//,
    //ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    tilesetImage = new QImage(QString("data/tileset.png"));

    if(tilesetImage)
        qDebug() << "Imagem carregada";
    else
        qDebug() << "Imagem não encontrada";


    tilesetEditor = new TilesetEditor(tilesetImage,this);

    tilesetDock = new QDockWidget(tr("Tileset"), this);
    tilesetDock->setWidget(tilesetEditor);
    tilesetDock->setMinimumWidth(200);

    addDockWidget(Qt::LeftDockWidgetArea, tilesetDock);

    mapView = new MapView(tilesetImage,this);
    setCentralWidget(mapView);


    QObject::connect(tilesetEditor, SIGNAL(statusTipUpdated(const QString &)),
                     this, SLOT(updateStatusBar(const QString &)) );

    QObject::connect(mapView, SIGNAL(mapChange()),
                     this, SLOT(mapWasModified()));

    QObject::connect(mapView, SIGNAL(statusTipUpdated(const QString &)),
                     this, SLOT(updateStatusBar(const QString &)) );

    QObject::connect(tilesetEditor, SIGNAL(targetTileChange(int,int)),
                     mapView, SLOT(targetTileChanged(int,int)));

    createActions();
    createToolBars();
    createMenus();
    createStatusBar();

    readSettings();

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);



}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if(maybeSave()) {
        setCurrentFile("");
        bool ok;
        qDebug() << "New save empty? " << curFile.isEmpty();
        int sizeX = QInputDialog::getInt(this, tr("Digite o tamanho do mapa"),
                                         tr("Largura:"), 12, 1, 2147483647, 1, &ok);
        if(!ok) return;

        int sizeY = QInputDialog::getInt(this, tr("Digite o tamanho do mapa"),
                                         tr("Altura:"), 12, 1, 2147483647, 1, &ok);

        if(!ok) return;

        mapView->newMap(QSize(sizeX,sizeY));

    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()){
            mapView->load(fileName);
        }
    }
}

bool MainWindow::save() {
    if(curFile.isEmpty()) {
        qDebug() << "saveAs";
        return saveAs();
    } else {
        qDebug("saveFile");
        return saveFile(curFile);
    }
}
bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    qDebug() << "saveAs QFileDialog: " << fileName;
    if(fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Sobre Ferramenta"),
        tr("<b>Map Builder</b> é uma ferramenta para construção "
           "de mapas para serem utilizados pela aplicação de análise "
           " de dados de jogos."));
}

void MainWindow::mapWasModified()
{
    setWindowModified(true);
}

void MainWindow::updateStatusBar(const QString &text)
{
    statusBar()->showMessage(text);
    qDebug() << "Update!";
}

//----- Setup -----//
void MainWindow::createActions()
{
    newAction = new QAction(QIcon(":/images/new.png"), tr("&Novo mapa"),this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Cria um novo mapa"));
    connect(newAction, SIGNAL(triggered()), this,
            SLOT(newFile()));

    openAction = new QAction(QIcon(":/images/open.png"), tr("Abrir &mapa"),this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Abre o mapa"));
    connect(openAction, SIGNAL(triggered()), this,
            SLOT(open()));

    saveAction = new QAction(QIcon(":/images/save.png"), tr("&Salvar mapa"),this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Salva o mapa"));
    connect(saveAction, SIGNAL(triggered()), this,
            SLOT(save()));

    saveAsAction = new QAction(tr("&Salvar como..."),this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Salva o mapa com um novo nome"));
    connect(saveAsAction, SIGNAL(triggered()), this,
            SLOT(saveAs()));

    exitAction = new QAction(tr("Sai&r mapa"),this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Sai da ferramenta"));
    connect(exitAction, SIGNAL(triggered()), this,
            SLOT(close()));

    aboutAction = new QAction(tr("&Sobre"),this);
    aboutAction->setStatusTip(tr("Sobre a ferramenta"));
    connect(aboutAction, SIGNAL(triggered()), this,
            SLOT(about()));

    editPathAction = new QAction(QIcon(":/images/edit.png"),tr("Editar caminho"),this);
    editPathAction->setStatusTip(tr("Edita propriedades dos blocos, se passáveis ou não"));
    editPathAction->setShortcut(tr("TAB"));
    editPathAction->setCheckable(true);
    connect(editPathAction, SIGNAL(triggered()), mapView,
            SLOT(toogleEditMode()) );

    showPathAction = new QAction(QIcon(":/images/show.png"),tr("Mostrar caminhos"),this);
    showPathAction->setStatusTip(tr("Mostra propriedades dos blocos, se passáveis ou não"));
    showPathAction->setShortcut(tr("CTRL+H"));
    showPathAction->setCheckable(true);
    connect(showPathAction, SIGNAL(triggered()), mapView,
            SLOT(toogleShowPath()) );

    showGridAction = new QAction(QIcon(":/images/grid.png"),tr("Mostrar quadrados"),this);
    showGridAction->setStatusTip(tr("Mostra divisão entre os blocos"));
    showGridAction->setShortcut(tr("CTRL+G"));
    showGridAction->setCheckable(true);
    showGridAction->setChecked(true);
    connect(showGridAction, SIGNAL(triggered()), mapView,
            SLOT(toogleShowGrid()) );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Arquivo"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Editar"));


    viewMenu = menuBar()->addMenu(tr("Exi&bir"));

    viewFerramentasSubMenu = viewMenu->addMenu("&Barra de ferramentas");
    viewFerramentasSubMenu->addAction(fileToolBar->toggleViewAction());
    viewFerramentasSubMenu->addAction(editToolBar->toggleViewAction());

    helpMenu = menuBar()->addMenu(tr("A&juda"));
    helpMenu->addAction(aboutAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Arquivo"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("Editar"));
    editToolBar->addAction(editPathAction);
    editToolBar->addAction(showPathAction);
    editToolBar->addSeparator();

    editToolBar->addAction(showGridAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Pronto"));
}

void MainWindow::readSettings()
{
    QSettings settings("QtProject", "MapBuilder");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "MapBuilder");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    return true;
}
bool MainWindow::saveFile(const QString &fileName)
{
    qDebug() << "saveFile";
    if(fileName.isEmpty())
        return false;
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    QString shownName = curFile;
    if(curFile.isEmpty())
        shownName = "untitled";

    setWindowTitle(QString("%1 - %2[*]").arg("MapBuilder").arg(shownName));
}
QString MainWindow::strippedName(const QString &fullFileName)
//! [48] //! [49]
{
    return QFileInfo(fullFileName).fileName();
}

MainWindow::~MainWindow()
{
    //delete ui;
    delete tilesetImage;
}
