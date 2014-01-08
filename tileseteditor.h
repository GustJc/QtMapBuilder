#ifndef TILESETEDITOR_H
#define TILESETEDITOR_H

#include <QWidget>
#include <QGraphicsView>

#include <QStyleOptionViewItem>
class QImage;
class TilesetEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TilesetEditor(QImage* image, QWidget *parent = 0);


    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
protected:
    void mousePressEvent( QMouseEvent * event );
    bool eventFilter(QObject *dist, QEvent *event);


signals:
    void statusTipUpdated(const QString &text);

public slots:

private:
    QImage* m_tileset;

    QGraphicsView  *view;
    QGraphicsScene *scene;

    QGraphicsRectItem *selection;
    QGraphicsPixmapItem* imageItem;

    QPoint clickStart;
    QPoint clickEnd;

};

#endif // TILESETEDITOR_H
