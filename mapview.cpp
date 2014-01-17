#include "mapview.h"
#include "defines.h"
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
MapView::MapView(QImage *image, QWidget *parent) :
    QGraphicsView(parent)
{
    m_tileset = image;

    scene = new QGraphicsScene(0,0,256,256,this);

    this->setScene(scene);
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setEnabled(false);
    this->setMouseTracking(true);

    selection = scene->addRect(QRectF(0,0,0,0),QPen(Qt::red, 2));

    m_gridInterval = 16;
    isMouseHold     = false;
    startId = endId = 0;

    m_tilesetLen = qFloor(m_tileset->width() / m_gridInterval);

    m_showPath = m_editMode = false;
    m_ShowEntities = true;
    m_showGrid     = true;

    m_selectedEntityIndex = -1;

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
            if(mapHolder[x][y].gfx < 0)
                continue;
            if(mapHolder[x][y].type < 0)
                continue;

            /**** REMOVER quando alterar no engine para gravar com -1 ***/
            if(mapHolder[x][y].type <= 0) {
                if(m_showPath){
                    painter->setBrush(QBrush(Qt::red));
                    painter->setOpacity(0.5f);
                    painter->drawRect(x*m_gridInterval, y*m_gridInterval, m_gridInterval, m_gridInterval);
                    painter->setOpacity(1.0f);
                }
                continue;
            }
            /*** FIM ****/

            int idX = mapHolder[x][y].gfx % m_tilesetLen;
            int idY = mapHolder[x][y].gfx / m_tilesetLen;
            painter->drawImage(x*m_gridInterval, y*m_gridInterval, *m_tileset,
                               idX*m_gridInterval, idY*m_gridInterval,
                               m_gridInterval, m_gridInterval);
            // Desenha Path
            if(m_showPath) {
                if(mapHolder[x][y].type == TYPE_BLOCK || mapHolder[x][y].type < 0 )
                    painter->setBrush(QBrush(Qt::red));
                else
                    painter->setBrush(QBrush(Qt::green));

                painter->setOpacity(0.5f);

                painter->drawRect(x*m_gridInterval, y*m_gridInterval, m_gridInterval, m_gridInterval);

                painter->setOpacity(1.0f);
            }


        }
    }
    //Desenha Entidades
    if(m_ShowEntities) {
        for(int i = 0; i < entityHolder.size(); ++i) {
            int idX = entityHolder[i].mGfx % m_tilesetLen;
            int idY = entityHolder[i].mGfx / m_tilesetLen;
            painter->drawImage(entityHolder[i].mPos.x()*m_gridInterval, entityHolder[i].mPos.y()*m_gridInterval,
                               *m_tileset,
                               idX*m_gridInterval, idY*m_gridInterval,
                               m_gridInterval, m_gridInterval);
        }
    }

    // Desenha Grid
    if(m_showGrid) {
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
    if(m_editMode == false) {
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

        selection->setPos( destination*16 );
        selection->setRect(0,0,sizeX, sizeY);
        this->scene->update(this->viewport()->rect());
    }

    //Desenha se segurando mouse
    if(isMouseHold)
    {
        if(m_showPath) {
            if(changed_selection)
                paintTooglePath();
        } else
            paintMap(selection->x(), selection->y());
    }
}



void MapView::paintTooglePath()
{
    int pX = qFloor(selection->x()/m_gridInterval);
    int pY = qFloor(selection->y()/m_gridInterval);

    if(!isValidMapPosition(QPoint(pX,pY))) return;

    int& type = mapHolder[pX][pY].type;

    if(type == TYPE_BLOCK || type < 0 ) {
        type = TYPE_PASS;
    } else {
        type = TYPE_BLOCK;
    }
}


void MapView::mouseReleaseEvent(QMouseEvent *)
{
    isMouseHold = false;
}
//---------------- Mouse Press --------------------//
void MapView::mousePressEvent(QMouseEvent *event)
{
    QPointF mouse = this->mapToScene(event->pos());
    if(mouse.x() < 0 || mouse.y() < 0) return;
    if(mouse.x() >= scene->sceneRect().width() ||
       mouse.y() >= sceneRect().height()) return;

    if(m_showPath) {
        showPathMousePress(event);
        return;
    } else
    if(m_editMode) {
        editModeMousePress(event);
        return;
    }


    if(event->button() == Qt::LeftButton) {
        paintMap(selection->x(), selection->y());
        isMouseHold = true;
    }

    if(event->button() == Qt::RightButton) {
        int pX = qFloor(mouse.x() / m_gridInterval);
        int pY = qFloor(mouse.y() / m_gridInterval);

        startId = endId = mapHolder[pX][pY].gfx;
    }

    repaint();
    this->scene->update(this->viewport()->rect());
}
void MapView::showPathMousePress(QMouseEvent *)
{
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
        setSelectedEntityIndex();
        qDebug() << m_selectedEntityIndex;
    } else {
        emit(statusTipUpdated(""));
    }

}

void MapView::setSelectedEntityIndex()
{
    for(int i = 0; i < entityHolder.size();++i) {
        if( qFloor(selection->x()/m_gridInterval) == entityHolder[i].mPos.x() &&
                qFloor(selection->y()/m_gridInterval) == entityHolder[i].mPos.y() ) {
            m_selectedEntityIndex = i;
            return;
        }
    }
    m_selectedEntityIndex = -1;
}

/*
 *
 * Acoes de mapa
 *
 */

void MapView::paintMap(int mouseX, int mouseY)
{
    int pX = qFloor(mouseX/m_gridInterval);
    int pY = qFloor(mouseY/m_gridInterval);

    if(!isValidMapPosition( QPoint(pX, pY) ) ) {
        return;
    }
    //if( pX < 0 || pY < 0) return;
    //if( pX >= mapHolder.size() || pY >= mapHolder[0].size()) return;
    //Esta dento do mapa, pode pintar

    emit(mapChange());

    qDebug() << "startId: " << startId << "endId: " << endId;

    if(endId == startId || endId < 0) {
        mapHolder[pX][pY].gfx = startId;
    } else {

        int diffX = (endId   % m_tilesetLen) - (startId % m_tilesetLen );

        int diffY = qFloor(endId   / m_tilesetLen) - qFloor(startId / m_tilesetLen);

        qDebug() << "Multi-Paint diff: " << diffX << ", " << diffY;

        for(int ix = 0; ix <= diffX; ix++){
            for(int iy = 0; iy <= diffY;iy++) {
                if(pX+ix >= mapHolder.size()) continue;
                if(pY+iy >= mapHolder[0].size()) break;

                mapHolder[pX+ix][pY+iy].gfx = startId + ix + iy*m_tilesetLen;
            }
        }

    }
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

void MapView::newMap(const QSize mapSize)
{
    scene->setSceneRect(QRectF(0,0,mapSize.width()*m_gridInterval, mapSize.height()*m_gridInterval));
    this->setEnabled(true);
    emit(mapChange());
    createMap(mapSize);
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
    /*
    std::string str;

    int px, py, hp, mp, atk, def, range, speed,sprIdx, sprIdy, buff;
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

            Entity* ent = new Entity();
            ent->setPosition(px,py);
            ent->mHP = hp;
            ent->mAtk = atk;
            ent->mDef = def;
            ent->mRange = range;
            ent->mSpeedCost = speed;
            ent->changeSprite(sprIdx, sprIdy);

            ent->addToObjectList();

        }else
        if(strcmp(str.c_str(), "Gold:") == 0)
        {
            myfile >> px; myfile.ignore(3,',');
            myfile >> py; myfile.ignore(3,',');
            myfile >> hp; myfile.ignore(1,',');

            ResourceManager::ResourceControl.addGold(px,py,hp);
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

            Item* item = new Item();
            item->setPosition(px,py);
            item->mIsBuff = buff;
            item->mHp = hp;
            item->mMp = mp;
            item->mAtk = atk;
            item->mDef = def;
            item->changeSprite(sprIdx, sprIdy);
            item->addToObjectList();
        }
    }// Fim do arquivo
    */
    myfile.close();
}

void MapView::toogleEditMode()
{
    m_editMode = !m_editMode;
}

void MapView::toogleShowPath()
{
    m_showPath = !m_showPath;
    repaint();
    this->scene->update(this->viewport()->rect());
}

void MapView::toogleShowGrid()
{
    m_showGrid = !m_showGrid;
    repaint();
    this->scene->update(this->viewport()->rect());
}

bool MapView::isValidMapPosition(QPoint pos)
{
    if(pos.x() < 0 || pos.y() < 0  ) return false;
    if(pos.x() >= mapHolder.size() ) return false;
    if(pos.y() >= mapHolder[0].size() ) return false;

    return true;
}

void MapView::targetTileChanged(int startIndex, int endIndex)
{
    this->startId = startIndex;
    this->endId   = endIndex;
}

void MapView::save(const QString& filename)
{
    QFile file(filename);
}
