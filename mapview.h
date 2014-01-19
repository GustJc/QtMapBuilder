#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>
#include <QVector>
#include "entity.h"
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

    void editModeMousePress(QMouseEvent* event);
    void editModeMouseMoveEvent(QMouseEvent* event);

    void showPathMousePress(QMouseEvent* event);
    void showPathMouseMoveEvent(QMouseEvent* event);

    QImage* m_tileset;
signals:
    void mapChange();
    void statusTipUpdated(const QString &text);

public slots:
    void newMap(const QSize mapSize);
    void targetTileChanged(int startIndex, int endIndex);

    void save(const QString &filename);
    void load(const QString &filename);

    void toogleEditMode();
    void toogleShowPath();
    void toogleShowGrid();
private:
    bool isValidMapPosition(QPoint pos);
    QGraphicsScene* scene;
    QGraphicsRectItem *selection;
    int m_gridInterval;
    int startId;
    int endId;
    int entityId;
    bool pathId;
    QVector<QVector<Tile> > mapHolder;
    QVector<Entity> entityHolder;
    bool isMouseHold;

    int m_tilesetLen;

private:
    void setSelectedEntityIndex();
    int m_selectedEntityIndex;
private:
    void paintMap(int mouseX, int mouseY);
    void paintArea(int mouseX, int mouseY, int width, int height);
    void paintTooglePath();


};

#endif // MAPVIEW_H
