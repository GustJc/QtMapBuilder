#include "tileseteditor.h"
#include <QStyleOptionViewItem>
#include <QDebug>
#include <QGraphicsWidget>
#include <QMouseEvent>
#include <QtMath>
#include <QScrollBar>
#include "globals.h"
#include "defines.h"
TilesetEditor::TilesetEditor(QImage *image, QWidget *parent) :
    QGraphicsView(parent)
{
    m_tileset = image;

    scene = new QGraphicsScene(QRectF(0,0,256,256));
    scene->addRect(scene->sceneRect(),QPen(Qt::black), QBrush(Qt::black));

    imageItem = scene->addPixmap(QPixmap::fromImage(*image));

    selection = scene->addRect(QRectF(0,0,0,0),QPen(Qt::red, 2));

    //QPalette Pal(palette());
    //Pal.setColor(QPalette::Background, Qt::darkGray);

    //this->setPalette(Pal);
    setBackgroundBrush(QBrush(Qt::darkGray));
    this->setAutoFillBackground(true);

    clickStart.setX(-1);

    viewport()->installEventFilter(this);

    setScene(scene);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);

    scrollClick = false;


    g_tilesetMapTypeInfo.clear();

    g_tilesetMapTypeInfo.resize( qFloor(image->width()/16) );
    for(int i = 0; i < g_tilesetMapTypeInfo.size();++i){
        g_tilesetMapTypeInfo[i].resize(qFloor(image->height()/16));
        for(int j = 0; j < g_tilesetMapTypeInfo[0].size(); ++j) {
            g_tilesetMapTypeInfo[i][j] = TYPE_PASS;
        }
    }
}

void TilesetEditor::mousePressEvent(QMouseEvent *event)
{
    QPointF mouse = this->mapToScene(event->x(), event->y());
    qDebug() << mouse << "width: " << mouse.y() << " height: " << m_tileset->height();

    clickStart.setX( qFloor(mouse.x()/16) );
    clickStart.setY( qFloor(mouse.y()/16) );

    clickEnd = clickStart;

    selection->setPos(clickStart.x()*16,clickStart.y()*16);
    selection->setRect(0,0,16,16);

    if(g_showPath) {
        int pX = qFloor(selection->x()/16);
        int pY = qFloor(selection->y()/16);

        const int& type = g_tilesetMapTypeInfo[pX][pY];

        if(type == TYPE_BLOCK || type < 0 ) {
            pathId = true;
        } else {
            pathId = false;
        }

        paintTooglePath();
    }
}

bool TilesetEditor::eventFilter(QObject *, QEvent *event)
{
    if( event->type() == QEvent::MouseMove)
    {
        qDebug() << scrollClick;
        if(scrollClick || horizontalScrollBar()->isSliderDown() ||
                verticalScrollBar()->isSliderDown()) return false;
        if(clickStart.x() < 0) return false;

    } else
    if(event->type() == QEvent::MouseButtonPress){
        if(horizontalScrollBar()->isSliderDown()) {
            return true;
        }
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );
        QPointF mouse = this->mapToScene(mouseEvent->x(), mouseEvent->y());
        if(mouse.x() > m_tileset->width() || mouse.x() < 0 ) {
            scrollClick = true;
            return true;
        }
        if(mouse.y() > m_tileset->height() || mouse.y() < 0) {
            scrollClick = true;
            return true;
        }
    }

    return false;
}

void TilesetEditor::mouseReleaseEvent(QMouseEvent *)
{
    int targetStart = clickStart.x()  + clickStart.y()*int(qFloor(m_tileset->width()/16) );
    int targetEnd   = clickEnd.x()    + clickEnd.y()*int(qFloor(m_tileset->width()/16) );
    qDebug() << targetStart << "tEnd: " << targetEnd;
    emit(targetTileChange(targetStart, targetEnd));

    scrollClick = false;
}

void TilesetEditor::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    QPointF mouseFloat = this->mapToScene(mouseEvent->pos());
    QPoint mouse(mouseFloat.x(), mouseFloat.y() );
    qDebug() << mouse;

    if(mouse.x() > m_tileset->width() || mouse.x() < 0 ) {
        return;
    }
    if(mouse.y() > m_tileset->height() || mouse.y() < 0) {
        return;
    }
    //Pinta se segurando mouse
    if(g_showPath) {
        clickStart.setX( qFloor(mouse.x()/16) );
        clickStart.setY( qFloor(mouse.y()/16) );
        clickEnd.setX( qFloor(mouse.x()/16) );
        clickEnd.setY( qFloor(mouse.y()/16) );
        selection->setPos(clickStart.x()*16,clickStart.y()*16);
        paintTooglePath();
    } else {
        mouse.setX(qFloor(mouse.x()/16));
        mouse.setY(qFloor(mouse.y()/16));
        qDebug() << mouse;

        if(mouse.x() >= clickStart.x()){
            clickEnd.setX( qFloor(mouse.x()) );
        }
        else
            clickStart.setX( qFloor(mouse.x()) );

        if(mouse.y() >= clickStart.y()){
            clickEnd.setY( qFloor(mouse.y()));
        }
        else
            clickStart.setY( qFloor(mouse.y()) );


        QPoint selectionSize = (clickEnd - clickStart);
        selectionSize+= QPoint(1,1);
        qDebug() << "ClickEndStart: " << clickStart <<" end: " << clickEnd;
        selection->setPos(clickStart.x()*16,clickStart.y()*16);
        selection->setRect(0,0,selectionSize.x()*16,selectionSize.y()*16);
    }
    setStatusTip(QString("%1, %2").arg(clickStart.x()).arg(clickStart.y()));

    emit(statusTipUpdated(QString("%1, %2").arg(clickStart.x()).arg(clickStart.y())));
}

void TilesetEditor::drawForeground(QPainter *painter, const QRectF &)
{
    if(g_showPath) {
        for(int x = 0; x < g_tilesetMapTypeInfo.size(); ++x){
            for(int y = 0; y < g_tilesetMapTypeInfo[0].size(); ++y ){

                if(g_tilesetMapTypeInfo[x][y] == TYPE_BLOCK || g_tilesetMapTypeInfo[x][y] < 0 )
                    painter->setBrush(QBrush(Qt::red));
                else
                    painter->setBrush(QBrush(Qt::green));

                painter->setOpacity(0.5f);

                painter->drawRect(x*16, y*16, 16, 16);

                painter->setOpacity(1.0f);


            }//end for(y)
        }//end for(x)
    }
}

void TilesetEditor::forceUpdate()
{
    repaint();
    this->scene->update(this->viewport()->rect());
}

void TilesetEditor::paintTooglePath()
{
    int pX = qFloor(selection->x()/16);
    int pY = qFloor(selection->y()/16);

    if(pX < 0 || pY < 0) return;
    if(pX >= g_tilesetMapTypeInfo.size()) return;
    if(pY >= g_tilesetMapTypeInfo[0].size()) return;

    int& type = g_tilesetMapTypeInfo[pX][pY];

    if(pathId && type != TYPE_PASS) {
        type = TYPE_PASS;
        forceUpdate();
    } else if (!pathId && type != TYPE_BLOCK){
        type = TYPE_BLOCK;
        forceUpdate();
    }
}
