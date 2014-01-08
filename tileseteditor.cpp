#include "tileseteditor.h"
#include <QStyleOptionViewItem>
#include <QDebug>
#include <QGraphicsWidget>
#include <QMouseEvent>
TilesetEditor::TilesetEditor(QImage *image, QWidget *parent) :
    QWidget(parent)
{
    m_tileset = image;

    qDebug() << "Construido tilesetEditor";

    scene = new QGraphicsScene(QRectF(0,0,256,256));
    scene->addRect(scene->sceneRect(),QPen(Qt::black), QBrush(Qt::black));



    imageItem = scene->addPixmap(QPixmap::fromImage(*image));

    selection = scene->addRect(QRectF(0,0,0,0),QPen(Qt::red, 2));



    view = new QGraphicsView(scene,this);
    view->setScene(scene);

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::darkGray);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);

    clickStart.setX(-1);

    view->viewport()->installEventFilter(this);

}

void TilesetEditor::mousePressEvent(QMouseEvent *event)
{
    QPointF mouse = view->mapToScene(event->x(), event->y());

    clickStart.setX(mouse.x()/16);
    clickStart.setY(mouse.y()/16);

    clickEnd = clickStart;

    selection->setPos(clickStart.x()*16,clickStart.y()*16);
    selection->setRect(0,0,16,16);
}

bool TilesetEditor::eventFilter(QObject *, QEvent *event)
{
    if( event->type() == QEvent::MouseMove)
    {

        if(clickStart.x() < 0) return false;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );

        QPoint mouse(mouseEvent->pos());
        mouse /= 16;


        if(mouse.x() > clickStart.x()){
            clickEnd.setX(mouse.x());
        }
        else
            clickStart.setX(mouse.x());

        if(mouse.y() > clickStart.y()){
            clickEnd.setY(mouse.y());
        }
        else
            clickStart.setY(mouse.y());


        QPoint selectionSize = clickEnd - clickStart;
        selection->setPos(clickStart.x()*16,clickStart.y()*16);
        selection->setRect(0,0,selectionSize.x()*16,selectionSize.y()*16);

        setStatusTip(QString("%1, %2").arg(clickStart.x()).arg(clickStart.y()));

        emit(statusTipUpdated(QString("%1, %2").arg(clickStart.x()).arg(clickStart.y())));

    }

    return false;
}

void TilesetEditor::mouseReleaseEvent(QMouseEvent *)
{
    clickStart.setX(-1);
}

void TilesetEditor::mouseMoveEvent(QMouseEvent *)
{
    qDebug() << "MOUSE MOVE";

}

