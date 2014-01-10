#include "mapview.h"

#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>
MapView::MapView(QImage *image, QWidget *parent) :
    QGraphicsView(parent)
{
    m_tileset = image;

    scene = new QGraphicsScene(0,0,256,256,this);

    this->setScene(scene);
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setEnabled(false);

    m_gridInterval = 16;
}

void MapView::drawBackground(QPainter *painter, const QRectF &rect)
{
    if(!isEnabled()) return;

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::gray));
    painter->drawRect(rect);

    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(scene->sceneRect());

    qDebug() << rect;
    // Draw Grid
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


void MapView::newMap(QSize mapSize)
{
    scene->setSceneRect(QRectF(0,0,mapSize.width()*m_gridInterval, mapSize.height()*m_gridInterval));
    this->setEnabled(true);
    emit(mapChange());
    qDebug() << "new map";
}

void MapView::load(const QString filename)
{

}

void MapView::save()
{

}
