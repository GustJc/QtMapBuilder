#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsView>

class QGraphicsScene;
class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MapView(QImage* image, QWidget *parent = 0);

protected:
    void drawBackground(QPainter* painter, const QRectF & rect);

    QImage* m_tileset;
private:
    QGraphicsScene* scene;
signals:
    void mapChange();

public slots:
    void newMap(QSize mapSize);
    void load(const QString filename);
    void save();

};

#endif // MAPVIEW_H
