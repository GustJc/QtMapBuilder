#include "mapview.h"
#include "defines.h"

#include "entityadddialog.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QtMath>

#include <QTextStream>
#include <QFile>

#include <iostream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "globals.h"
MapView::MapView(QImage *image, QImage *character, QImage *itens, QWidget *parent) :
    QGraphicsView(parent)
{
    m_tileset = image;
    m_charset = character;
    m_itemset = itens;

    scene = new QGraphicsScene(0,0,256,256,this);

    this->setScene(scene);
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setEnabled(false);
    this->setMouseTracking(true);

    selection = scene->addRect(QRectF(0,0,0,0),QPen(Qt::red, 2));

    m_gridInterval = 16;
    isMouseHold     = false;
    startId = endId = entityId = 0;
    pathId = false;

    m_tilesetLen = qFloor(m_tileset->width() / m_gridInterval);

    g_showPath = g_editMode = g_rectangleTool = false;
    g_paintTool    = true;

    g_isClickActive = false;

    hasCopyTile = false;

    m_selectedEntityHolderIndex = m_selectedEntityListIndex = -1;

}


void MapView::drawBackground(QPainter *painter, const QRectF &rect)
{
    if(!isEnabled()) return;

    //Desenha cores de fundo
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::gray));
    painter->drawRect(rect.x()+rect.width()  - this->rect().width(),
                      rect.y()+rect.height() - this->rect().height(),
                      this->rect().width(), this->rect().height());
    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(scene->sceneRect());

    // Desenha Mapa
    for(int x = 0; x < mapHolder.size(); ++x) {
        for(int y = 0; y < mapHolder[x].size(); ++y) {
                        // Desenha mapa
            if(mapHolder[x][y].type < 0)
                continue;

            /**** REMOVER quando alterar no engine para gravar com -1 ***
            if(mapHolder[x][y].type <= 0) {
                if(g_showPath){
                    painter->setBrush(QBrush(Qt::red));
                    painter->setOpacity(0.5f);
                    painter->drawRect(x*m_gridInterval, y*m_gridInterval, m_gridInterval, m_gridInterval);
                    painter->setOpacity(1.0f);
                }
                continue;
            }
            *** FIM ****/
            if(mapHolder[x][y].gfx >= 0) {
                int idX = mapHolder[x][y].gfx % m_tilesetLen;
                int idY = qFloor(mapHolder[x][y].gfx / m_tilesetLen);
                painter->drawImage(x*m_gridInterval, y*m_gridInterval, *m_tileset,
                                   idX*m_gridInterval, idY*m_gridInterval,
                                   m_gridInterval, m_gridInterval);
            }
            // Desenha Path
            if(g_showPath) {
                if(mapHolder[x][y].type == TYPE_BLOCK || mapHolder[x][y].type < 0 )
                    painter->setBrush(QBrush(Qt::red));
                else
                    painter->setBrush(QBrush(Qt::green));

                painter->setOpacity(0.5f);

                painter->drawRect(x*m_gridInterval, y*m_gridInterval, m_gridInterval, m_gridInterval);

                painter->setOpacity(1.0f);
            }
            if(mapHolder[x][y].type == TYPE_START) {
                painter->drawImage(x*m_gridInterval, y*m_gridInterval, QImage(":/images/startCursor.png"));
            } else if(mapHolder[x][y].type == TYPE_END) {
                painter->drawImage(x*m_gridInterval, y*m_gridInterval, QImage(":/images/endCursor.png"));
            }


        }
    }
    //Desenha itens e chars
    for(int i = 0; i < entityHolder.size();++i) {
        const int& type = entityHolder.at(i).typeId();
        if(type == ENTITY_ENEMY && g_showChars == false)
            continue;
        else if(type == ENTITY_ITEM || type == ENTITY_GOLD)
                if(g_ShowItens == false) continue;

        QImage* image = m_itemset;
        if(type == ENTITY_ENEMY) image = m_charset;

        int tilesetLen = qFloor(image->width()/m_gridInterval);
        QPoint idPos = getPosFromGfx(entityHolder[i].mGfx, tilesetLen);

        painter->drawImage(entityHolder[i].mPos.x()*m_gridInterval, entityHolder[i].mPos.y()*m_gridInterval,
                           *image,
                           idPos.x()*m_gridInterval, idPos.y()*m_gridInterval,
                           m_gridInterval, m_gridInterval);
    }

    //Desenha RectangleTool
    if(g_rectangleTool && g_isClickActive)
    {
        int sizeX = qFloor(selection->rect().width()/m_gridInterval);
        int sizeY = qFloor(selection->rect().height()/m_gridInterval);
        int startX = qFloor(selection->pos().x()/m_gridInterval);
        int startY = qFloor(selection->pos().y()/m_gridInterval);
        for(int x = startX; x < mapHolder.size() &&
                        x < startX+sizeX; ++x) {
            for(int y = startY; y < mapHolder[x].size() &&
                        y < startY+sizeY; ++y) {
                int idX = startId % m_tilesetLen;
                int idY = qFloor(startId / m_tilesetLen);
                painter->drawImage(x*m_gridInterval, y*m_gridInterval, *m_tileset,
                                   idX*m_gridInterval, idY*m_gridInterval,
                                   m_gridInterval, m_gridInterval);
            }
        }
    }

    // Desenha Grid
    if(g_showGrid) {
        painter->setPen(QPen(QColor(0,0,0,50)));
        const int gridInterval = 16;
        qreal left = int(scene->sceneRect().left()) - (int(scene->sceneRect().left()) % gridInterval );
        qreal top = int(scene->sceneRect().top()) - (int(scene->sceneRect().top()) % gridInterval );
        QVarLengthArray<QLineF, gridInterval> linesX;
        for (qreal x = left; x <= scene->sceneRect().right(); x += gridInterval )
            linesX.append(QLineF(x, scene->sceneRect().top(), x, scene->sceneRect().bottom()));

        QVarLengthArray<QLineF, gridInterval> linesY;
        for (qreal y = top; y <= scene->sceneRect().bottom(); y += gridInterval )
                linesY.append(QLineF(scene->sceneRect().left(), y, scene->sceneRect().right(), y));

        painter->drawLines(linesX.data(), linesX.size());
        painter->drawLines(linesY.data(), linesY.size());
    }




}

void MapView::wheelEvent(QWheelEvent *event)
{   //From: http://www.qtcentre.org/wiki/index.php?title=QGraphicsView:_Smooth_Panning_and_Zooming
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
}

/*
 * -----------------------------
 * Events
 *-----------------------------
 */
void MapView::mouseMoveEvent(QMouseEvent *event)
{
    // Tamanho da area de seleção
    int idYStart = startId / m_tilesetLen;
    int idXStart = (startId % m_tilesetLen);

    int idYEnd = endId / m_tilesetLen;
    int idXEnd = (endId % m_tilesetLen);

    int sizeX = (1+abs( idXStart - idXEnd) )* m_gridInterval;
    int sizeY = (1+abs( idYStart - idYEnd) )* m_gridInterval;

    bool changed_selection = false;
    //Move selecao e redesenha
    if(g_editMode == false) {
        QPointF mouseFloat = this->mapToScene(event->pos());
        QPoint  destination(mouseFloat.x(), mouseFloat.y());
        destination.setX(qFloor(mouseFloat.x()/16));
        destination.setY(qFloor(mouseFloat.y()/16));

        if(isValidMapPosition(destination) ) {
            Tile& t = mapHolder[destination.x()][destination.y()];
            emit(statusTipUpdated(tr("Tile[%1][%2]: Tipo %3 Gfx %4").arg(destination.x()).arg(destination.y())
                              .arg(t.type).arg(t.gfx)));
        } else {
            emit(statusTipUpdated(""));
        }

        if(qFloor(selection->x()/m_gridInterval)  != destination.x() ||
           qFloor(selection->y()/m_gridInterval)  != destination.y() ) {
            changed_selection = true;
        }

        //Seleção não move quando retangleTool ativo
        if(g_isClickActive == false) {
            selection->setPos( destination*16 );

            if(g_rectangleTool == false && !g_startTool && !g_endTool)
                selection->setRect(0,0,sizeX, sizeY);
            else
                selection->setRect(0,0,m_gridInterval, m_gridInterval);

            this->scene->update(this->viewport()->rect());
        } else if(g_rectangleTool)
        {  //Ajusta tamanho da seleção
            QPoint size(m_gridInterval + destination.x()*16 - selection->pos().x(),
                        m_gridInterval + destination.y()*16 - selection->pos().y());

            selection->setRect(0,0,size.x(), size.y());
        }
    }

    //Pinta se segurando mouse
    if(isMouseHold)
    {
        if(g_showPath) {
            if(changed_selection)
                paintTooglePath();
        } else if(g_paintTool){
                paintMap(selection->x(), selection->y());
        }
    }
}

void MapView::paintTooglePath()
{
    hasCopyTile = false;
    int pX = qFloor(selection->x()/m_gridInterval);
    int pY = qFloor(selection->y()/m_gridInterval);

    if(!isValidMapPosition(QPoint(pX,pY))) return;

    int& type = mapHolder[pX][pY].type;

    if(pathId)
        type = TYPE_PASS;
    else
        type = TYPE_BLOCK;
}

void MapView::mouseReleaseEvent(QMouseEvent *)
{
    isMouseHold = false;
}

void MapView::mouseDoubleClickEvent(QMouseEvent *)
{
    if(g_cursorTool && g_editMode) {
        EntityAddDialog edit(this);
        if(m_selectedEntityHolderIndex < 0)
            return;

        edit.setEntity(entityHolder[m_selectedEntityHolderIndex]);
        edit.exec();
    }
}
//---------------- Mouse Press --------------------//
void MapView::mousePressEvent(QMouseEvent *event)
{
    QPointF mouse = this->mapToScene(event->pos());
    if(mouse.x() < 0 || mouse.y() < 0) return;
    if(mouse.x() >= scene->sceneRect().width() ||
       mouse.y() >= sceneRect().height()) return;

    if(g_showPath) {
        showPathMousePress(event);
        return;
    } else
    if(g_editMode) {
        editModeMousePress(event);
        return;
    }

    if(event->button() == Qt::LeftButton) {
        if(g_rectangleTool) {
            if(g_isClickActive == false) {
                g_isClickActive = true;
                isMouseHold = true;
                return;
            } else {
                paintArea(selection->x(), selection->y(), selection->rect().width(), selection->rect().height());
                g_isClickActive = false;
                return;
            }
        }
        paintMap(selection->x(), selection->y());
        mouseMoveEvent(event); //Atualiza statusBar depois de mudar tipo sem mexer mouse, remover?
        isMouseHold = true;
    }

    //TODO, ajustar tamanho para 1 tile ao clicar, trocar para normal ao perder hasCopyTIle
    //TODO, Permitir copia em área ao arrastar com botão direito segurado.
    if(event->button() == Qt::RightButton) {
        if(g_rectangleTool) {
            g_isClickActive = false;
        } else {
            int pX = qFloor(mouse.x() / m_gridInterval);
            int pY = qFloor(mouse.y() / m_gridInterval);

            hasCopyTile = true;
            copyTile    = mapHolder[pX][pY];
            if(copyTile.type == TYPE_START){
                copyTile.type = TYPE_PASS;
            }
        }
    }

    repaint();
    this->scene->update(this->viewport()->rect());
}
void MapView::showPathMousePress(QMouseEvent *)
{
    int pX = qFloor(selection->x()/m_gridInterval);
    int pY = qFloor(selection->y()/m_gridInterval);

    const int& type = mapHolder[pX][pY].type;

    if(type == TYPE_BLOCK || type < 0 ) {
        pathId = true;
        //type = TYPE_PASS;
    } else {
        pathId = false;
        //type = TYPE_BLOCK;
    }

    paintTooglePath();
    isMouseHold = true;

    repaint();
    this->scene->update(this->viewport()->rect());
}

void MapView::editModeMousePress(QMouseEvent *event)
{
    QPointF mouseFloat = this->mapToScene(event->pos());
    QPoint  destination(mouseFloat.x(), mouseFloat.y());
    destination.setX(qFloor(mouseFloat.x()/16));
    destination.setY(qFloor(mouseFloat.y()/16));

    if(isValidMapPosition(destination) ) {
        Tile& t = mapHolder[destination.x()][destination.y()];
        emit(statusTipUpdated(tr("Tile[%1][%2]: Tipo %3 Gfx %4").arg(destination.x()).arg(destination.y())
                          .arg(t.type).arg(t.gfx)));
        selection->setPos( destination*16 );
        selection->setRect(0,0,m_gridInterval, m_gridInterval);
        this->scene->update(this->viewport()->rect());
        if(g_cursorTool)
            setSelectedEntityIndexFromClick();
        else
            addEntity(selection->pos().toPoint(),m_selectedEntityListIndex);

        qDebug() << m_selectedEntityHolderIndex;
    } else {
        emit(statusTipUpdated(""));
    }

}

void MapView::setSelectedEntityIndexFromClick()
{
    for(int i = 0; i < entityHolder.size();++i) {
        if( qFloor(selection->x()/m_gridInterval) == entityHolder[i].mPos.x() &&
                qFloor(selection->y()/m_gridInterval) == entityHolder[i].mPos.y() ) {
            m_selectedEntityHolderIndex = i;
            return;
        }
    }
    m_selectedEntityHolderIndex = -1;
}

/*
 *
 * Acoes de mapa
 *
 */

void MapView::paintMap(int mouseX, int mouseY)
{   //Ativa quando clicar ou segurar esquerdo e g_paintTool
    int pX = qFloor(mouseX/m_gridInterval);
    int pY = qFloor(mouseY/m_gridInterval);

    if(!isValidMapPosition( QPoint(pX, pY) ) ) {
        return;
    }
    //if( pX < 0 || pY < 0) return;
    //if( pX >= mapHolder.size() || pY >= mapHolder[0].size()) return;
    //Esta dento do mapa, pode pintar

    emit(mapChange());

    if(g_startTool) {
        if(isMouseHold) return;
        qDebug() << "StartTool Active";
        if(mapHolder[pX][pY].type == TYPE_START) mapHolder[pX][pY].type = TYPE_PASS;
        else {
            QPoint startpoint = getPlayerPosition();
            //Se houver um ponto de inicio, troca
            if(startpoint.x() >= 0 || startpoint.y() >= 0){
                mapHolder[startpoint.x()][startpoint.y()].type = TYPE_PASS;
            }

            mapHolder[pX][pY].type = TYPE_START;
        }
        return;
    } else if(g_endTool) {
        if(isMouseHold) return;
        qDebug() << "EndTool Active";
        if(mapHolder[pX][pY].type == TYPE_END) mapHolder[pX][pY].type = TYPE_PASS;
        else mapHolder[pX][pY].type = TYPE_END;
        return;
    }

    qDebug() << "startId: " << startId << "endId: " << endId;
    if(hasCopyTile) {
        mapHolder[pX][pY] = copyTile;
    } else
    if(endId == startId || endId < 0) {
        mapHolder[pX][pY].gfx = startId;
        mapHolder[pX][pY].type = g_tilesetMapTypeInfo[(startId   % m_tilesetLen)][qFloor(startId   / m_tilesetLen)];
    } else {

        int diffX = (endId   % m_tilesetLen) - (startId % m_tilesetLen );

        int diffY = qFloor(endId   / m_tilesetLen) - qFloor(startId / m_tilesetLen);

        qDebug() << "Multi-Paint diff: " << diffX << ", " << diffY;

        for(int ix = 0; ix <= diffX; ix++){
            for(int iy = 0; iy <= diffY;iy++) {
                if(pX+ix >= mapHolder.size()) continue;
                if(pY+iy >= mapHolder[0].size()) break;

                mapHolder[pX+ix][pY+iy].gfx  = startId + ix + iy*m_tilesetLen;
                mapHolder[pX+ix][pY+iy].type = g_tilesetMapTypeInfo[(startId   % m_tilesetLen)+ix][qFloor(startId   / m_tilesetLen)+iy];
            }
        }

    }
}


void MapView::paintArea(int mouseX, int mouseY, int width, int height)
{
    hasCopyTile = false;
    int pX = qFloor(mouseX/m_gridInterval);
    int pY = qFloor(mouseY/m_gridInterval);
    int pW = qFloor(width/m_gridInterval);
    int pH = qFloor(height/m_gridInterval);

    if(!isValidMapPosition( QPoint(pX, pY) ) ) {
        return;
    }

    emit(mapChange());

    for(int x = pX; x < mapHolder.size() &&
                    x < pX+pW; ++x) {
        for(int y = pY; y < mapHolder[x].size() &&
                    y < pY+pH; ++y) {
            mapHolder[x][y].gfx = startId;
        }
    }
}


bool MapView::isValidEntityPos(QPoint pos)
{
    //valid pos?
    for(int i = 0; i < entityHolder.size();++i) {
        if(entityHolder[i].mPos == pos){
            qDebug() << "Posição da entidade inválida.";
            return false;
        }
    }

    return true;
}

bool MapView::isValidEntityAbsPos(QPoint absPos)
{
    QPoint pos(qFloor(absPos.x()/m_gridInterval),qFloor(absPos.y()/m_gridInterval));
    return isValidEntityPos(pos);
}

void MapView::addEntity(QPoint absPos, int index)
{
    if(index < 0 || index >= g_entitylist.size()) return;

    if( isValidEntityAbsPos(absPos) == false ) return;

    QPoint pos(qFloor(absPos.x()/m_gridInterval),qFloor(absPos.y()/m_gridInterval));

    g_entitylist[index].mPos = pos;
    entityHolder.push_back(g_entitylist[index]);
}

void MapView::createMap(const QSize mapSize)
{
    for(int i = 0; i < mapHolder.size();++i)
        mapHolder[i].clear();

    mapHolder.clear();

    mapHolder.resize(mapSize.width());
    for(int i = 0; i < mapHolder.size();++i)
        mapHolder[i].resize(mapSize.height());

    for(int x = 0; x < mapSize.width(); ++x){
        for(int y = 0; y < mapSize.height(); ++y) {
            Tile t;
            t.gfx   = TILE_NONE;
            t.type = TYPE_PASS;
            mapHolder[x][y] = t;
        }
    }
}

void MapView::setToolSelection(int tool, bool isOn)
{
    if(isOn == false) {
        qDebug() << "OffCURSOR!";
        this->setCursor(Qt::ArrowCursor);
    } else {
        if(tool == START_TOOL) {
            this->setCursor(QCursor(QPixmap(":/images/startCursor.png")));
        }
        if(tool == END_TOOL) {
            this->setCursor(QCursor(QPixmap(":/images/endCursor.png")));
        }


    }

}

QPoint MapView::getPosFromGfx(int gfx, const int tilesetLen)
{
    return QPoint(qFloor(gfx % tilesetLen), qFloor(gfx / tilesetLen));
}

void MapView::setTile(int x, int y, int tileGfx, int tileType)
{
    if(mapHolder.empty()) return;
    if( x < 0 || x >= mapHolder.size()) return;
    if( y < 0 || y >= mapHolder[0].size()) return;

    if(tileGfx >= 0)
        mapHolder[x][y].gfx = tileGfx;
    if(tileType >= 0 )
        mapHolder[x][y].type = tileType;
}

Tile* MapView::getTile(int x, int y)
{
    if(isValidMapPosition(QPoint(x,y)) == false) return NULL;

    return &mapHolder[x][y];
}

void MapView::setEntity(int x, int y, Entity ent)
{
    // Verifica se a posição ficará dentro do mapa
    if(mapHolder.empty()) return;
    if( x < 0 || x >= mapHolder.size()) return;
    if( y < 0 || y >= mapHolder[0].size()) return;

    if(isValidEntityPos(QPoint(x,y)) == false) return;


    ent.mPos.setX(x);
    ent.mPos.setY(y);

    entityHolder.push_back(ent);
}

void MapView::setOverrideEntity(int x, int y, Entity ent)
{
    // Verifica se a posição ficará dentro do mapa
    if(mapHolder.empty()) return;
    if( x < 0 || x >= mapHolder.size()) return;
    if( y < 0 || y >= mapHolder[0].size()) return;

    QPoint pos(x,y);

    ent.mPos.setX(x);
    ent.mPos.setY(y);

    for(int i = 0; i < entityHolder.size();++i) {
        if(entityHolder[i].mPos == pos){
            entityHolder.remove(i);
            break;
        }
    }

    entityHolder.push_back(ent);
}

int MapView::getMapHeight()
{
    if(mapHolder.empty()) return 0;
    return mapHolder[0].size();
}

int MapView::getMapWidth()
{
    return mapHolder.size();
}

void MapView::forceUpdateMap()
{
    repaint();
    this->scene->update(this->viewport()->rect());
}

MapView::MapView()
{
    //For Luabind
}

void MapView::newMap(const QSize mapSize)
{
    scene->setSceneRect(QRectF(0,0,mapSize.width()*m_gridInterval, mapSize.height()*m_gridInterval));
    this->setEnabled(true);
    emit(mapChange());
    entityHolder.clear();
    createMap(mapSize);
}

void MapView::newMapInt(int w, int h)
{
    newMap(QSize(w,h));
}

void MapView::toogleEditMode()
{
    g_editMode = !g_editMode;
    hasCopyTile = false;
}

void MapView::toogleShowPath()
{
    g_showPath = !g_showPath;
    repaint();
    this->scene->update(this->viewport()->rect());
    hasCopyTile = false;
}

void MapView::toogleShowGrid()
{
    g_showGrid = !g_showGrid;
    repaint();
    this->scene->update(this->viewport()->rect());
}

void MapView::toogleShowItem()
{
    g_ShowItens = !g_ShowItens;
    repaint();
    this->scene->update(this->viewport()->rect());
}

void MapView::toogleShowEnemy()
{
    g_showChars = !g_showChars;
    repaint();
    this->scene->update(this->viewport()->rect());
}

void MapView::entityListSelectionChanged(int index)
{
    m_selectedEntityListIndex = index;
}

bool MapView::isValidMapPosition(QPoint pos)
{
    if(mapHolder.empty()) return false;
    if(pos.x() < 0 || pos.y() < 0  ) return false;
    if(pos.x() >= mapHolder.size() ) return false;
    if(pos.y() >= mapHolder[0].size() ) return false;

    return true;
}

void MapView::targetTileChanged(int startIndex, int endIndex)
{
    this->startId = startIndex;
    this->endId   = endIndex;

    hasCopyTile = false;
}



void MapView::load(const QString& filename)
{
    using namespace std;
    ifstream myfile;
    string absFileName(filename.toStdString());
    myfile.open( absFileName.c_str());//string("./data/map/"+filename).c_str() );

    if(myfile.is_open() == false){
        printf("Nao foi possivel abrir o arquivo: %s.\n", absFileName.c_str());
        return;
    }

    int map_width = -1, map_height = -1;

    int player_x = 0;
    int player_y = 0;
    myfile >> player_x;
    myfile.ignore(3,'.');
    myfile >> player_y;

    myfile >> map_width;
    myfile.ignore(20,'-');
    myfile >> map_height;


    //Ajusta o maior tamanho do mapa
    if(map_width < 0 || map_height < 0) {
        qDebug() << "Tamanho invalido do mapa: " << map_width << ", " << map_height;
        return;
    }

    newMap(QSize(map_width, map_height));

    for (int y = 0; y < map_height; y++)
    {
        for (int x = 0; x < map_width; x++)
        {   // Le Tiles e joga-os no TileList
            Tile tempTile;
            myfile >> tempTile.type; myfile.ignore(1,':');
            myfile >> tempTile.gfx;


            mapHolder[x][y] = tempTile;
        }
    }//Fim preencher mapa

    //Player::PlayerControl->setPosition(player_x, player_y);

    myfile.ignore(20,'\n');

    std::string str;

    int px, py, hp, mp, atk, def, range, speed,sprIdx, sprIdy, buff, gfx;
    while (myfile.eof() == false) //Le a linha
    {
        myfile >> str;
        if(strcmp(str.c_str(), "Enemy:") == 0)
        {
            myfile >> px; myfile.ignore(3,',');
            myfile >> py; myfile.ignore(3,',');
            myfile >> hp; myfile.ignore(3,',');
            myfile >> atk; myfile.ignore(3,',');
            myfile >> def; myfile.ignore(3,',');
            myfile >> range; myfile.ignore(3,',');
            myfile >> speed; myfile.ignore(3,',');
            myfile >> sprIdx; myfile.ignore(3,',');
            myfile >> sprIdy; myfile.ignore(1,',');

            gfx = sprIdx + sprIdy*qFloor(m_charset->width()/m_gridInterval);

            Entity ent;
            ent.setEnemy(gfx,hp,atk,def,range,speed,mp);
            ent.mPos.setX(px);
            ent.mPos.setY(py);

            entityHolder.push_back(ent);

        }else
        if(strcmp(str.c_str(), "Gold:") == 0)
        {
            myfile >> px; myfile.ignore(3,',');
            myfile >> py; myfile.ignore(3,',');
            myfile >> hp; myfile.ignore(1,',');

            Entity ent;
            ent.setGold(hp);
            ent.mPos.setX(px);
            ent.mPos.setY(py);

            entityHolder.push_back(ent);
        } else
        if(strcmp(str.c_str(), "Item:") == 0)
        {
            myfile >> px; myfile.ignore(3,',');
            myfile >> py; myfile.ignore(3,',');
            myfile >> buff; myfile.ignore(3,',');
            myfile >> hp; myfile.ignore(3,',');
            myfile >> mp; myfile.ignore(3,',');
            myfile >> atk; myfile.ignore(3,',');
            myfile >> def; myfile.ignore(3,',');
            myfile >> sprIdx; myfile.ignore(3,',');
            myfile >> sprIdy; myfile.ignore(1,',');

            gfx = sprIdx + sprIdy*qFloor(m_itemset->width()/m_gridInterval);

            Entity ent;

            ent.setItem(gfx, hp, mp, atk, def, speed);
            ent.mIsBuff = buff;
            ent.mPos.setX(px);
            ent.mPos.setY(py);

            entityHolder.push_back(ent);
        }
    }// Fim do arquivo

    myfile.close();
}


QPoint MapView::getPlayerPosition()
{
    for(int i = 0; i < mapHolder.size();++i) {
        for(int j = 0; j < mapHolder[0].size(); ++j) {
            if(mapHolder[i][j].type == TYPE_START) {
                return QPoint(i,j);
            }
        }
    }
    return QPoint(-1,-1);
}
void MapView::save(const QString& filename)
{
    std::ofstream file;
    std::string absFileName(filename.toStdString());
    file.open( absFileName.c_str(),std::ios::out | std::ios::binary);

    int map_width = getMapWidth();
    int map_height = getMapHeight();
    QPoint player_pos = getPlayerPosition();
    file << player_pos.x() << "." << player_pos.y() << "\n";
    file << map_width << "-" << map_height << "\n";

    for (int y = 0; y < map_height; y++)
    {
        for (int x = 0; x < map_width; x++)
        {

            file << mapHolder[x][y].type << ":" << mapHolder[x][y].gfx;
            file << " ";
        }
        file << "\n";
    }
    file << "\n";

    //Salva inimigos e itens
    /**
    for(unsigned int i = 0; i < ObjectList.size(); ++i)
    {
        if(ObjectList[i]->type == TYPE_ENEMY)
        {
            file << "Enemy:\n";
            Entity* ent = (Entity*)ObjectList[i];
            file << ent->getPositionX() << ',' << ent->getPositionY() << ','
                    << ent->mHP << ',' << ent->mAtk << ',' << ent->mDef << ','
                    << ent->mRange << ',' << ent->mSpeedCost << ','
                    << ent->getSpriteIdx() << ',' << ent->getSpriteIdy() << '\n';

        }
        else
        if(ObjectList[i]->type == TYPE_ITEM)
        {
            Item* item = (Item*)ObjectList[i];
            if(item->mGold != 0)
            {
                file << "Gold:\n";
                file << item->getPositionX() << ',' << item->getPositionY() << ','
                        << item->mGold << '\n';
            }
            else
            {
                file << "Item:\n";
                file << item->getPositionX() << ',' << item->getPositionY() << ','
                        << item->mIsBuff << ',' << item->mHp << ','
                        << item->mMp << ',' << item->mAtk << ',' << item->mDef << ','
                        << item->getSpriteIdx() << ',' << item->getSpriteIdy() << '\n';
            }
        } //Fim item
    } //Fim for
    **/
    file << "\n";
    file.close();
}
