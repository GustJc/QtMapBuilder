#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QtGui>
#include "tileseteditor.h"
#include "mapview.h"
#include "scriptselectorwidget.h"
#include "globals.h"
#include "entityadddialog.h"
#include "scriptmainwindow.h"
#include <QDebug>
#include <QTreeWidgetItem>
#include "entityadddialog.h"
#include "luamanager.h"
void MainWindow::connectObjects()
{
    QObject::connect(tilesetEditor, SIGNAL(statusTipUpdated(const QString &)),
                     this, SLOT(updateStatusBar(const QString &)) );

    QObject::connect(mapView, SIGNAL(mapChange()),
                     this, SLOT(mapWasModified()));

    QObject::connect(mapView, SIGNAL(statusTipUpdated(const QString &)),
                     this, SLOT(updateStatusBar(const QString &)) );

    QObject::connect(tilesetEditor, SIGNAL(targetTileChange(int,int)),
                     mapView, SLOT(targetTileChanged(int,int)));

    QObject::connect(entitySelector, SIGNAL(itemSelectionChanged()), this,
                              SLOT(entityItemChanged()));
    QObject::connect(entitySelector, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this,
                              SLOT(entityItemDoubleClicked(QTreeWidgetItem*,int)));

    QObject::connect(this, SIGNAL(entityListSelectionChange(int)), mapView,
                     SLOT(entityListSelectionChanged(int)));

    QObject::connect(scriptSelector, SIGNAL(runScriptClicked(QString)), this,
                     SLOT(runMapScript(QString)));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)//,
    //ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    tilesetImage = new QImage(QString("data/tileset.png"));
    enemyImage   = new QImage(QString("data/chars.png"));
    itemImage    = new QImage(QString("data/itens.png"));

    if(tilesetImage)
        qDebug() << "Imagem carregada";
    else
        qDebug() << "Imagem não encontrada";


    //Tileset editor setup
    tilesetEditor = new TilesetEditor(tilesetImage,this);

    tilesetDock = new QDockWidget(tr("Tileset"), this);
    tilesetDock->setWidget(tilesetEditor);
    tilesetDock->setMinimumWidth(200);

    addDockWidget(Qt::LeftDockWidgetArea, tilesetDock);

    mapView = new MapView(tilesetImage, enemyImage, itemImage,this);
    setCentralWidget(mapView);

    createActions();
    createToolBars();
    createMenus();
    createStatusBar();

    readSettings();

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);

    entitySelector = new QTreeWidget(this);
    QStringList columnNames;
        columnNames << "Entidade" << "Tipo";
    entitySelector->setHeaderLabels(columnNames);
    entitySelector->setRootIsDecorated(false);
    entitySelector->setColumnCount(2);
    entitySelector->setMouseTracking(true);

    entitySelector->setSelectionMode(QAbstractItemView::SingleSelection);

    entitySelector->setVisible(false);

    populateTreeList();

    //tilesetDock->setWidget(entitySelector);


    connectObjects();

//    QMap<QString,QString> mMap;
//    mMap.insert("key", "gust");
//    mMap.insert("my","name");

//    QString str = mMap.find("my").value();
//    qDebug() << str;
//    str = mMap.find("key").value();
//    qDebug() << str;
//    if (mMap.find("a") == mMap.end())
//        qDebug() << "END";


    scriptWindow = new ScriptMainWindow(this);
    LuaManager::LuaControl.startLua(mapView);

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
}

//----- Setup -----//
void MainWindow::createActions()
{
    newAction = new QAction(QIcon(":/images/new.png"), tr("&Novo mapa"),this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Cria um novo mapa"));
    connect(newAction, SIGNAL(triggered()), this,
            SLOT(newFile()));

    newEntityAction = new QAction(QIcon(":/images/new.png"), tr("&Nova entidade"),this);
    newEntityAction->setStatusTip(tr("Cria uma nova entidade"));
    connect(newEntityAction, SIGNAL(triggered()), this,
            SLOT(addNewEntity()));


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

    editModeAction = new QAction(QIcon(":/images/edit.png"),tr("Editar entidades"),this);
    editModeAction->setStatusTip(tr("Edita propriedades dos blocos, se passáveis ou não"));
    editModeAction->setShortcut(tr("TAB"));
    editModeAction->setCheckable(true);
    connect(editModeAction, SIGNAL(triggered()), mapView,
            SLOT(toogleEditMode()) );
    connect(editModeAction, SIGNAL(triggered(bool)), this,
            SLOT(toogleEditMode(bool)) );

    showPathAction = new QAction(QIcon(":/images/show.png"),tr("Mostrar caminhos"),this);
    showPathAction->setStatusTip(tr("Mostra propriedades dos blocos, se passáveis ou não"));
    showPathAction->setShortcut(tr("CTRL+H"));
    showPathAction->setCheckable(true);
    connect(showPathAction, SIGNAL(triggered()), mapView,
            SLOT(toogleShowPath()) );
    connect(showPathAction, SIGNAL(triggered()), tilesetEditor,
            SLOT(forceUpdate()));

    showGridAction = new QAction(QIcon(":/images/grid.png"),tr("Mostrar quadrados"),this);
    showGridAction->setStatusTip(tr("Mostra divisão entre os blocos"));
    showGridAction->setShortcut(tr("CTRL+G"));
    showGridAction->setCheckable(true);
    showGridAction->setChecked(true);
    connect(showGridAction, SIGNAL(triggered()), mapView,
            SLOT(toogleShowGrid()) );

    showEnemyAction = new QAction(QIcon(":/images/enemy.png"),tr("Mostrar inimigos"),this);
    showEnemyAction->setStatusTip(tr("Mostra inimigos no mapa"));
    showEnemyAction->setShortcut(tr("CTRL+E"));
    showEnemyAction->setCheckable(true);
    showEnemyAction->setChecked(true);
    connect(showEnemyAction, SIGNAL(triggered()), mapView,
            SLOT(toogleShowEnemy()) );

    showItemAction = new QAction(QIcon(":/images/item.png"),tr("Mostrar itens"),this);
    showItemAction->setStatusTip(tr("Mostra itens no mapa"));
    showItemAction->setShortcut(tr("CTRL+I"));
    showItemAction->setCheckable(true);
    showItemAction->setChecked(true);
    connect(showItemAction, SIGNAL(triggered()), mapView,
            SLOT(toogleShowItem()) );

    //Tools
    paintToolAction = new QAction(QIcon(":/images/paint.png"),tr("Pincel"),this);
    paintToolAction->setStatusTip(tr("Pinta quadrados"));
    paintToolAction->setShortcut(tr("F"));
    paintToolAction->setCheckable(true);
    paintToolAction->setChecked(true);
    connect(paintToolAction, SIGNAL(triggered(bool)), this,
            SLOT(tooglePaintTool(bool)) );

    cursorToolAction = new QAction(QIcon(":/images/cursor.png"),tr("Selecionar"),this);
    cursorToolAction->setStatusTip(tr("Seleciona e edita entidades no modo edição"));
    cursorToolAction->setShortcut(tr("S"));
    cursorToolAction->setCheckable(true);
    cursorToolAction->setChecked(false);
    cursorToolAction->setEnabled(false);
    connect(cursorToolAction, SIGNAL(triggered(bool)), this,
            SLOT(toogleCursorTool(bool)) );

    rectangleToolAction = new QAction(QIcon(":/images/rectanglePaint.png"),tr("Retangulo"),this);
    rectangleToolAction->setStatusTip(tr("Pinta em uma area retangular"));
    rectangleToolAction->setShortcut(tr("SHIFT+F"));
    rectangleToolAction->setCheckable(true);
    rectangleToolAction->setChecked(false);
    connect(rectangleToolAction, SIGNAL(triggered(bool)), this,
            SLOT(toogleRectangleTool(bool)) );

    startToolAction = new QAction(QIcon(":/images/start.png"),tr("Ponto de Inicio"),this);
    startToolAction ->setStatusTip(tr("Coloca ponto de entrada do mapa(somente 1)"));
    startToolAction ->setShortcut(tr("I"));
    startToolAction ->setCheckable(true);
    startToolAction ->setChecked(false);
    connect(startToolAction, SIGNAL(triggered(bool)), this,
            SLOT(toogleStartTool(bool)) );

    endToolAction = new QAction(QIcon(":/images/end.png"),tr("Pontos de Saída"),this);
    endToolAction ->setStatusTip(tr("Coloca pontos de saída do mapa"));
    endToolAction ->setShortcut(tr("E"));
    endToolAction ->setCheckable(true);
    endToolAction ->setChecked(false);
    connect(endToolAction, SIGNAL(triggered(bool)), this,
            SLOT(toogleEndTool(bool)) );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Arquivo"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(newEntityAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Editar"));
    editMenu->addAction(cursorToolAction);
    editMenu->addAction(paintToolAction);
    editMenu->addAction(rectangleToolAction);
    editMenu->addSeparator();
    editMenu->addAction(startToolAction);
    editMenu->addAction(endToolAction);

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
    editToolBar->addAction(cursorToolAction);
    editToolBar->addAction(paintToolAction);
    editToolBar->addAction(rectangleToolAction);

    editToolBar->addSeparator();
    editToolBar->addAction(editModeAction);
    editToolBar->addAction(showPathAction);
    editToolBar->addAction(startToolAction);
    editToolBar->addAction(endToolAction);
    editToolBar->addSeparator();

    editToolBar->addAction(showGridAction);
    editToolBar->addAction(showEnemyAction);
    editToolBar->addAction(showItemAction);

    scriptSelector = new ScriptSelectorWidget(this);
    editToolBar->addSeparator();
    editToolBar->addSeparator();
    editToolBar->addWidget(scriptSelector);
    editToolBar->addSeparator();
    editToolBar->addSeparator();
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
bool MainWindow::saveFile(const QString &filename)
{
    qDebug() << "saveFile";
    if(filename.isEmpty())
        return false;

    mapView->save(filename);

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

void MainWindow::entityItemChanged()
{
    QTreeWidgetItem* item = entitySelector->selectedItems().first();

    qDebug() << item->text(0);
    qDebug() << item->text(1);
    qDebug() << entitySelector->currentIndex().row();
    emit(entityListSelectionChange(item->text(2).toInt()));
}

void MainWindow::entityItemDoubleClicked(QTreeWidgetItem *item, int )
{
    qDebug() << item->text(0);
    qDebug() << item->text(1);
    int index = item->text(2).toInt();

    EntityAddDialog edit(this);
    if(index < 0 || index >= g_entitylist.size())
        return;

    edit.setEntity(g_entitylist[index]);
    edit.exec();

}

void MainWindow::addNewEntity()
{
    QStringList types;
    types << "Inimigo" << "Dinheiro" << "Item";
    bool isOK = false;
    QString itemChoosen = QInputDialog::getItem(this,tr("Selecione o tipo de entidade"),"Selecione o tipo de entidade: ", types,
                                                0, false, &isOK);
    qDebug() << itemChoosen;

    if( !isOK ) {
        return;
    }

    Entity ent;

    if(itemChoosen.contains("Inimigo")) {
        ent.setEnemy(0,10,10,10,10,10);
    } else if(itemChoosen.contains("Dinheiro")) {
        ent.setGold(10);
    } else if(itemChoosen.contains("Item")) {
        ent.setItem(0, 10, 10, 10, 10, 10);
    } else {
        return;
    }

    EntityAddDialog edit(this);

    edit.setEntity(ent);

    if( edit.exec() == QDialog::Accepted ) {
        g_entitylist.push_back(ent);
        updateTreeListData();
    }
}

void MainWindow::tooglePaintTool(bool isChecked)
{
    if(isChecked) {
        g_paintTool     = true;
        g_cursorTool    = false;
        g_rectangleTool = false;
        g_isClickActive = false;
        cursorToolAction->setChecked(false);
        rectangleToolAction->setChecked(false);
    } else {
        paintToolAction->setChecked(true);
    }
}

void MainWindow::toogleRectangleTool(bool isChecked)
{
    if(isChecked) {
        g_paintTool     = false;
        g_cursorTool    = false;
        g_rectangleTool = true;
        g_isClickActive = false;
        paintToolAction->setChecked(false);
        cursorToolAction->setChecked(false);
    } else {
        rectangleToolAction->setChecked(true);
    }
}

void MainWindow::toogleEditMode(bool isChecked)
{
    //DockWidget
    if(isChecked)
    {
        tilesetDock->setWidget(entitySelector);
        entitySelector->setVisible(true);
        tilesetEditor->setVisible(false);
        tilesetDock->setWindowTitle(tr("Entity"));
    } else
    {
        tilesetDock->setWidget(tilesetEditor);
        tilesetEditor->setVisible(true);
        entitySelector->setVisible(false);
        tilesetDock->setWindowTitle(tr("Tileset"));
    }

    //Tools
    if(isChecked) {
        cursorToolAction->setEnabled(true);
    } else {
        if(cursorToolAction->isChecked()){
            g_paintTool     = true;
            g_cursorTool    = false;
            g_rectangleTool = false;
            g_isClickActive = false;
            paintToolAction->setChecked(true);
            cursorToolAction->setChecked(false);
        }

        cursorToolAction->setEnabled(false);


    }
}

void MainWindow::runMapScript(QString filename)
{
    qDebug() << "Script: " << filename;
    //LuaManager::LuaControl.executeTests();

    scriptWindow->show();
    scriptWindow->setFocus();
    scriptWindow->openFile(filename);

    if(filename.isEmpty() == false)
        LuaManager::LuaControl.doFile(filename);
}

void MainWindow::toogleStartTool(bool isChecked)
{
    if(isChecked) {
        g_startTool         = true;
        g_endTool           = false;
        g_rectangleTool     = false;
        g_paintTool         = true;
        updateActionsStates();
    } else {
        g_startTool         = false;
    }

    mapView->setToolSelection(START_TOOL, isChecked);
}

void MainWindow::toogleEndTool(bool isChecked)
{
    if(isChecked) {
        g_startTool         = false;
        g_endTool           = true;
        g_rectangleTool     = false;
        g_paintTool         = true;
        updateActionsStates();
    } else {
        g_endTool           = false;
    }

    mapView->setToolSelection(END_TOOL, isChecked);
}

void MainWindow::toogleCursorTool(bool isChecked)
{
    if(isChecked) {
        g_paintTool     = false;
        g_cursorTool    = true;
        g_rectangleTool = false;
        g_isClickActive = false;
        paintToolAction->setChecked(false);
        rectangleToolAction->setChecked(false);
    } else {
        cursorToolAction->setChecked(true);
    }
}

void MainWindow::populateTreeList()
{
    // Futuramente, extrair itens dos scripts .lua

    Entity ent;
    ent.setEnemy(2,100,50,20,4,100);
    ent.mName = "Gobler";

    Entity ent2;
    ent2.setItem(1,100,100,10,0,0);
    ent2.mName = "Mega Staff";

    g_entitylist.push_back(ent);
    g_entitylist.push_back(ent2);


    updateTreeListData();

}

void MainWindow::updateTreeListData()
{
    entitySelector->clear();

    for(int i = 0; i < g_entitylist.size(); ++i) {
        QTreeWidgetItem* item = new QTreeWidgetItem(entitySelector);
        item->setText(0, g_entitylist.at(i).mName);
        item->setText(2, QString::number(i));
        switch(g_entitylist.at(i).typeId()) {
        case ENTITY_ENEMY:
            item->setText(1, tr("Inimigo"));
            item->setToolTip(0,
                             "<table>" +
                             QString("<tr><td><b>HP:</b></td>     <td>%1</td></tr>"
                                     "<tr><td><b>MP:</b></td>     <td>%2</td></tr>"
                                     "<tr><td><b>Ataque:</b></td> <td>%3</td></tr>"
                                     "<tr><td><b>Defesa:</b></td> <td>%4</td></tr>"
                                     "<tr><td><b>Range:</b></td>  <td>%5</td></tr>"
                                     "<tr><td><b>Delay:</b></td>  <td>%6</td></tr>"
                                     )
                                .arg(g_entitylist.at(i).mHp).arg(g_entitylist.at(i).mMp)
                                .arg(g_entitylist.at(i).mAtk).arg(g_entitylist.at(i).mDef)
                                .arg(g_entitylist.at(i).mRange).arg(g_entitylist.at(i).mDelay)+
                             "</table>"
                             );
            item->setToolTip(1, item->toolTip(0));

            break;
        case ENTITY_ITEM:
            item->setText(1, tr("Item"));
            item->setToolTip(0,
                             "<table>" +
                             QString("<tr><td><b>HP:</b></td>     <td>%1</td></tr>"
                                     "<tr><td><b>MP:</b></td>     <td>%2</td></tr>"
                                     "<tr><td><b>Ataque:</b></td> <td>%3</td></tr>"
                                     "<tr><td><b>Defesa:</b></td> <td>%4</td></tr>"
                                     )
                                .arg(g_entitylist.at(i).mHp).arg(g_entitylist.at(i).mMp)
                                .arg(g_entitylist.at(i).mAtk).arg(g_entitylist.at(i).mDef) +
                             "</table>"
                             );
            item->setToolTip(1, item->toolTip(0));
            break;
        case ENTITY_GOLD:
            item->setText(1, tr("Dinheiro"));
            item->setToolTip(0,
                             "<table>" +
                             QString("<tr><td><b>Gold:</b></td>     <td>%1</td></tr>"
                                     )
                                .arg(g_entitylist.at(i).mGold)+
                             "</table>"
                             );
            item->setToolTip(1, item->toolTip(0));
            break;
        default:
            item->setText(1, tr("Indefinido"));
        }
//        item->setToolTip(0,
//                     "<html>"
//                     "<div style=\"32px; height: 32px; overflow: hidden;\">"
//                        "<img src='data/chars.png' style=\"position: absolute; left:-37px;\"/>"
//                     "</div>"
//                     "</html>");
    }
}
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

MainWindow::~MainWindow()
{
    //delete ui;
    delete tilesetImage;
    delete enemyImage;
    delete itemImage;
}

void MainWindow::updateActionsStates()
{
    rectangleToolAction->setChecked(g_rectangleTool);
    paintToolAction->setChecked(g_paintTool);
    cursorToolAction->setChecked(g_cursorTool);
    endToolAction->setChecked(g_endTool);
    startToolAction->setChecked(g_startTool);
    showPathAction->setChecked(g_showPath);
    showGridAction->setChecked(g_showGrid);
    showEnemyAction->setChecked(g_showChars);
    showItemAction->setChecked(g_ShowItens);
    editModeAction->setChecked(g_editMode);

//  g_ShowEntities;

}
