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
    explicit MapView(QImage* image, QImage* character, QImage* itens, QWidget *parent = 0);

    void createMap(const QSize mapSize, int defaultType, int defaultGfx);
    void setToolSelection(int tool, bool isOn);

    QPoint getPosFromGfx(int gfx, const int tilesetLen);
    //Compatibility
    QPoint getPlayerPosition();
    //Map for Lua
    void newMapInt(int w, int h, int defaultType = 1, int defaultGfx = -1);
    void setTile(int x, int y, int tileGfx, int tileType);
    Tile *getTile(int x, int y);
    void setEntity(int x, int y, Entity& ent);
    void setOverrideEntity(int x, int y, Entity ent);
    int getMapHeight();
    int getMapWidth();
    void forceUpdateMap();
    MapView(); // Para luabind, nunca usadao
protected:
    void drawBackground(QPainter* painter, const QRectF & rect);
    void wheelEvent(QWheelEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *);

    void editModeMousePress(QMouseEvent* event);

    void showPathMousePress(QMouseEvent* event);
signals:
    void mapChange();
    void statusTipUpdated(const QString &text);

public slots:
    void newMap(const QSize mapSize, int defaultType = 1, int defaultGfx = -1);
    void targetTileChanged(int startIndex, int endIndex);

    void save(const QString &filename);
    void load(const QString &filename);

    void toogleEditMode();
    void toogleShowPath();
    void toogleShowGrid();
    void toogleShowItem();
    void toogleShowEnemy();
    void entityListSelectionChanged(int index);
private:
    bool isValidMapPosition(QPoint pos);
    QGraphicsScene* scene;
    QGraphicsRectItem *selection;
    int m_gridInterval;
    int startId;
    int endId;
    int entityId;
    bool hasCopyTile;
    Tile copyTile;
    bool pathId;
    bool isMouseHold;

    QVector<QVector<Tile> > mapHolder;
    QVector<Entity> entityHolder;
    QImage* m_tileset;
    QImage* m_charset;
    QImage* m_itemset;


    int m_tilesetLen;

private:
    void setSelectedEntityIndexFromClick();
    int m_selectedEntityHolderIndex;

    int m_selectedEntityListIndex;
private: //Tools
    void paintMap(int mouseX, int mouseY);
    void paintArea(int mouseX, int mouseY, int width, int height);
    void paintTooglePath();
public:
    bool isValidEntityPos(QPoint pos);
    bool isValidEntityAbsPos(QPoint absPos);
    void addEntity(QPoint absPos, int index);


};

#endif // MAPVIEW_H
