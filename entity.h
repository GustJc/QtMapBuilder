#ifndef ENTITY_H
#define ENTITY_H
enum {
    ENTITY_ENEMY = 1,
    ENTITY_ITEM  = 2,
    ENTITY_GOLD  = 3
};
#include <QPoint>
#include <QString>
class Entity
{
public:
    Entity();
    void setTypeId(int type);
    int typeId() const;

    void setEnemy(int gfx, int hp, int atk, int def, int range, int speed, int mp=0);
    void setGold(int gold);
    void setItemRecover(int gfx, int hp, int mp);
    void setItemStatus(int gfx, int atk, int def, int speed=0);
    void setItem(int gfx, int hp, int mp, int atk, int def, int speed);

    bool isValidPos(QPoint newPos);

    QPoint mPos;

    int mGfx;
//Inimigos
    int mHp;
    int mMp;
    int mAtk;
    int mDef;
    int mRange;
    int mDelay;
    QString mName;
//Item
    //mAtk
    //mDef
    //mHp
    //mMp
//Gold
    int mGold;
    bool mIsBuff;

private:
    int m_typeId;

};

#endif // ENTITY_H
