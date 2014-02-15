#ifndef TILESETEDITOR_H
#define TILESETEDITOR_H

#include <QWidget>
#include <QGraphicsView>

#include <QStyleOptionViewItem>
class QImage;
class ScrollArea;
class TilesetEditor : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TilesetEditor(QImage* image, QWidget *parent = 0);

protected:
    void mousePressEvent( QMouseEvent * event );
    bool eventFilter(QObject *dist, QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void statusTipUpdated(const QString &text);
    void targetTileChange(int startIndex, int endIndex);

public slots:

private:
    QImage* m_tileset;

    QGraphicsScene *scene;

    QGraphicsRectItem *selection;
    QGraphicsPixmapItem* imageItem;

    QPoint clickStart;
    QPoint clickEnd;

    bool scrollClick;
};

#endif // TILESETEDITOR_H
