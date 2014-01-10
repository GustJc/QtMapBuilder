#include "mapview.h"

#include <QGraphicsScene>
#include <QDebug>
MapView::MapView(QImage *image, QWidget *parent) :
    QGraphicsView(parent)
{
    m_tileset = image;

    scene = new QGraphicsScene(0,0,256,256, this);

    this->setScene(scene);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setEnabled(false);
}

void MapView::drawBackground(QPainter *painter, const QRectF &rect)
{
    if(!isEnabled()) return;
    // Draw Grid
    painter->setPen(QPen(QColor(0,0,0,50)));
    const int gridInterval = 16;
    qreal left = int(rect.left()) - (int(rect.left()) % gridInterval );
    qreal top = int(rect.top()) - (int(rect.top()) % gridInterval );
    QVarLengthArray<QLineF, gridInterval> linesX;
    for (qreal x = left; x < rect.right(); x += gridInterval )
        linesX.append(QLineF(x, rect.top(), x, rect.bottom()));

    QVarLengthArray<QLineF, gridInterval> linesY;
    for (qreal y = top; y < rect.bottom(); y += gridInterval )
            linesY.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(linesX.data(), linesX.size());
    painter->drawLines(linesY.data(), linesY.size());
}


void MapView::newMap(QSize mapSize)
{
    scene->setSceneRect(QRectF(0,0,mapSize.width(), mapSize.height()));
    setEnabled(true);
    emit(mapChange());
    qDebug() << "new map";
}

void MapView::load(const QString filename)
{

}

void MapView::save()
{

}
