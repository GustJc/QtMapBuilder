#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QVector>
struct Tile {
    int gfx;
    int type;
};

class QGraphicsScene;
class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapView(QImage* image, QWidget *parent = 0);

    void createMap(const QSize mapSize);
protected:
    void drawBackground(QPainter* painter, const QRectF & rect);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintMap(int mouseX, int mouseY);

    QImage* m_tileset;
signals:
    void mapChange();
    void statusTipUpdated(const QString &text);

public slots:
    void newMap(const QSize mapSize);
    void targetTileChanged(int startIndex, int endIndex);

    void save(const QString &filename);
    void load(const QString &filename);
private:
    bool isValidMapPosition(QPoint pos);
    QGraphicsScene* scene;
    QGraphicsRectItem *selection;
    int m_gridInterval;
    int startId;
    int endId;
    QVector<QVector<Tile> > mapHolder;
    bool m_showGrid;
    bool isMouseHold;

    int m_tilesetLen;
};

#endif // MAPVIEW_H
