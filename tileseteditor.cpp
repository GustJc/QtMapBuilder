#include "tileseteditor.h"
#include <QStyleOptionViewItem>
#include <QDebug>
#include <QGraphicsWidget>
#include <QMouseEvent>
#include <QtMath>
#include <QScrollBar>
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
}

bool TilesetEditor::eventFilter(QObject *, QEvent *event)
{
    if( event->type() == QEvent::MouseMove)
    {
        qDebug() << scrollClick;
        if(scrollClick || horizontalScrollBar()->isSliderDown() ||
                verticalScrollBar()->isSliderDown()) return false;
        if(clickStart.x() < 0) return false;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );

        QPointF mouseFloat = this->mapToScene(mouseEvent->pos());
        QPoint mouse(mouseFloat.x(), mouseFloat.y() );
        qDebug() << mouse;

        if(mouse.x() > m_tileset->width() || mouse.x() < 0 ) {
            return false;
        }
        if(mouse.y() > m_tileset->height() || mouse.y() < 0) {
            return false;
        }

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

        setStatusTip(QString("%1, %2").arg(clickStart.x()).arg(clickStart.y()));

        emit(statusTipUpdated(QString("%1, %2").arg(clickStart.x()).arg(clickStart.y())));

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

void TilesetEditor::mouseMoveEvent(QMouseEvent *)
{
    qDebug() << "MOUSE MOVE";

}

