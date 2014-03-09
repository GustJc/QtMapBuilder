#include "scriptmainwindow.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qsciapis.h>
#include <Qsci/qscistyle.h>
#include <QFileDialog>
#include <QTreeWidget>
#include <QSplitter>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QAction>
#include <QToolBar>
#include <qdebug.h>
#include "luamanager.h"
ScriptMainWindow::ScriptMainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    initializeUI();
    initializeEditor();
    initializeLexer();
    initializeActions();
}

void ScriptMainWindow::openFile(const QString filename)
{
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly))
        return;

    this->curFile = filename;
    editor->setText(file.readAll());
    setCurrentFile(filename);

    file.close();
}

void ScriptMainWindow::setCurrentFile(const QString filename)
{
    this->curFile = filename;
    QString shownName = curFile;
    if(curFile.isEmpty())
        shownName = "untitled";

    setWindowTitle(QString("%1 - %2[*]").arg("Lua Scripter").arg(shownName));
}

void ScriptMainWindow::initializeActions()
{
    executeScriptAction = new QAction(style()->standardIcon(QStyle::SP_ArrowRight), tr("&Executa script. (CTRL+R)"),this);
    executeScriptAction->setShortcut(tr("CTRL+R"));
    executeScriptAction->setStatusTip(tr("Executa script"));
    connect(executeScriptAction, SIGNAL(triggered()), this,
            SLOT(onExecuteScript()));

    testScriptAction = new QAction(style()->standardIcon(QStyle::SP_BrowserReload), tr("&Testa script. (CTRL+E)"),this);
    testScriptAction->setShortcut(tr("CTRL+E"));
    testScriptAction->setStatusTip(tr("Testa script, não o executa"));
    connect(testScriptAction, SIGNAL(triggered()), this,
            SLOT(onTestScript()));

    saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), tr("&Salva script. (CTRL+S)"),this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Salva script"));
    connect(saveAction, SIGNAL(triggered()), this,
            SLOT(onSaveFile()));

    btnToolBar = addToolBar(tr("Barra de Ferramentas"));
    btnToolBar->addAction(saveAction);
    btnToolBar->addSeparator();
    btnToolBar->addAction(testScriptAction);
    btnToolBar->addAction(executeScriptAction);

}

void ScriptMainWindow::initializeUI()
{
    setCurrentFile("");

    editor = new QsciScintilla(this);
    treeSelector = new QTreeWidget(this);
    treeSelector->header()->close();
    treeSelector->setRootIsDecorated(false);

    treeSelector->resize(200,600);
    editor->resize(500,600);

    QSplitter* split = new QSplitter(this);

    split->addWidget(editor);
    split->addWidget(treeSelector);
    split->setHandleWidth(10);

    QTreeWidgetItem* item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "sleep(int milliseconds)");
        item = new QTreeWidgetItem(treeSelector);

    item->setText(0, "______________________________________________________________________");

        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "map:createMap (int width, int height, int defaultType, int defaultGfx)");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "map:setTile (int posX, int posY, int gfx, int tileType)");
        item = new QTreeWidgetItem(treeSelector);

    item->setText(0, "______________________________________________________________________");

        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:clearEntityMap()");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:addGold(int px, int py, int gold)");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:addEntityByIndex(int px, int py, int ID)");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:addItemByIndex(int px, int py, int ID)");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:addEntity(int px, int py, std::string name)");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:addItem(int px, int py, std::string   name)");
        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "res:addCustomEntity(int px, int py, Entity& ent)");
        item = new QTreeWidgetItem(treeSelector);

    item->setText(0, "______________________________________________________________________");

        item = new QTreeWidgetItem(treeSelector);
    item->setText(0, "Tile.PASS, Tile.BLOCK, Tile.START, Tile.END");

    this->setCentralWidget(split);
    this->adjustSize();
    this->resize(700,600);
}

void ScriptMainWindow::initializeEditor()
{


    editor->markerDefine(QImage(":images/endCursor.png"), 0 );
    //Page
    editor->setPaper(QColor("#fbffcb"));
    editor->setIndentationGuidesBackgroundColor(QColor("#e6e6de"));

    editor->setCaretWidth(10);
    editor->setCaretLineBackgroundColor(QColor("#e6fff0"));
    editor->setCaretLineVisible(true);
    editor->ensureLineVisible(1);
    editor->setTabWidth(2);

    editor->setUtf8(true);
    //editor->setWhitespaceVisibility(QsciScintilla::WsVisible);

    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);

    //Folding
    QsciScintilla::FoldStyle state =
            static_cast<QsciScintilla::FoldStyle>((!editor->folding()) * 5);
    if(!state)
        editor->foldAll(false);

    editor->setFolding(state);

    //Margin
    QFontMetrics fontmetrics = QFontMetrics(editor->font());

    editor->setMarginsFont(editor->font());

    editor->setMarginWidth(0, fontmetrics.width("0000"));

    editor->setMarginLineNumbers(0, true);

    editor->setEdgeMode(QsciScintilla::EdgeLine);
    editor->setEdgeColumn(80);
    editor->setEdgeColor(QColor("#FF0000"));

    editor->setMarginsBackgroundColor(QColor("#cccccc"));

    connect(editor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    //Font
    QFont font("Courier", 12);
    font.setFixedPitch(true);
    editor->setFont(font);
}

void ScriptMainWindow::initializeLexer()
{
    QsciLexerLua *lexer = new QsciLexerLua();
    lexer->setDefaultFont(editor->font());
    lexer->setFoldCompact(false);


    editor->setAutoIndent(true);
    editor->setAutoCompletionThreshold(2);
    editor->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    QsciAPIs* apis = new QsciAPIs(lexer);
//    apis->add("createMap?1(int width, int height);");
//    apis->add("createTile?2(int tileId);?1");
//    apis->add("createMaps?3(int tileId);?2");
//    apis->add("foobar //isCool");

    apis->add("sleep(int milliseconds) \nPausa o programa por X milisegundos");
    apis->add("map:createMap (int width, int height, int defaultType, int defaultGfx) \nCria o mapa para ser desenhado.");
    apis->add("map:setTile (int posX, int posY, int gfx, int tileType) \nMuda o gráfico e o tipo do tile escolhido.");

    apis->add("res:clearEntityMap()\nLimpa lista de entidades.");
    apis->add("res:addGold(int px, int py, int gold)\nAdiciona dinheiro ao mapa.");
    apis->add("res:addEntityByIndex(int px, int py, int ID)\nAdiciona entidade pelo ID da lista ao mapa.");
    apis->add("res:addItemByIndex(int px, int py, int ID)\ndiciona item pelo ID da lista ao mapa.");
    apis->add("res:addEntity(int px, int py, std::string name)\nAdiciona entidade pelo nome ao mapa.");
    apis->add("res:addItem(int px, int py, std::string   name)\nAdiciona item pelo nome ao mapa.");
    apis->add("res:addCustomEntity(int px, int py, Entity& ent)\nAdiciona uma entidade customizada ao mapa.");

    apis->add("Tile.PASS Bloco passável.");
    apis->add("Tile.BLOCK Bloco não passável.");
    apis->add("Tile.START Início da fase, só deve haver um.");
    apis->add("Tile.END Fim da fase, pode haver múltiplos.");
    apis->prepare();

    editor->setLexer(lexer);
}

void ScriptMainWindow::onTextChanged()
{
    QFontMetrics fontmetrics = editor->fontMetrics();

    editor->setMarginWidth(0, fontmetrics.width(QString::number(editor->lines())));
}

void ScriptMainWindow::onExecuteScript()
{
    if(curFile.isEmpty()) return;

    QStringList strList = LuaManager::LuaControl.doFile(curFile);

    displayCodeErrors(strList);

}

void ScriptMainWindow::onTestScript()
{
    if(curFile.isEmpty()) return;

    QStringList strList = LuaManager::LuaControl.loadFile(curFile);

    displayCodeErrors(strList);
}

void ScriptMainWindow::displayCodeErrors(const QStringList strList)
{
    editor->clearAnnotations();
    editor->markerDeleteAll();

    foreach (QString str, strList) {
        qDebug() << str << "!!!!!";
        int index = str.indexOf(QRegularExpression(":\\d+"));
        int lastIndex = str.indexOf(QRegularExpression(": "));

        int lineIndex = str.indexOf(QRegularExpression("line+ \\d+"));
        qDebug() << "lineIndex: " << lineIndex;
        int lineLastIndex = str.indexOf(") ");
        qDebug() << "lineLastIndex: " << lineLastIndex;

        index++;
        lineIndex+= QString("line ").length();

        int lineStartError = str.mid(lineIndex, (lineLastIndex-lineIndex)).toInt() -1;
        int lineEndError = str.mid(index,(lastIndex-index)).toInt() -1;

        if(lineStartError < 0) lineStartError = 0;
        if(lineEndError < 0) lineEndError = 1;

        editor->annotate(lineStartError, str.mid(lineLastIndex+2), 15);
        editor->annotate(lineEndError, str.mid(lastIndex+2), 15);

        editor->setAnnotationDisplay(editor->AnnotationBoxed);

        editor->markerAdd(lineStartError, 0);
        editor->markerAdd(lineEndError, 0);
    }
}

void ScriptMainWindow::onOpen()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open .lua file"),
                                                    QDir::currentPath(),
                                                    "LUA (*.lua)");
    if(filename.isEmpty())
        return;
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly))
        return;

    editor->setText(file.readAll());
    file.close();
}

void ScriptMainWindow::onSaveFile()
{
    if(curFile.isEmpty())
        return;

    QFile file(curFile);

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "Not write only";
        return;
    }

    QTextStream out(&file);

    out << editor->text();
    file.close();
}
