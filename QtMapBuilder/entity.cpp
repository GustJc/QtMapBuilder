#include "entity.h"

Entity::Entity()
{
}

void Entity::setTypeId(int type)
{
    m_typeId = type;
}

int Entity::typeId() const
{
    return m_typeId;
}

void Entity::setEnemy(int gfx, int hp, int atk, int def, int range, int speed, int mp)
{
    mGfx = gfx;
    mHp = hp;
    mAtk = atk;
    mDef = def;
    mRange = range;
    mDelay = speed;
    mMp = mp;
    m_typeId = ENTITY_ENEMY;
}

void Entity::setGold(int gold)
{
    mGfx = 0;
    mGold = gold;
    m_typeId = ENTITY_GOLD;
}

void Entity::setItemRecover(int gfx, int hp, int mp)
{
    mGfx = gfx;
    mHp = hp;
    mMp = mp;
    m_typeId = ENTITY_ITEM;
}

void Entity::setItemStatus(int gfx, int atk, int def, int speed)
{
    mGfx = gfx;
    mAtk = atk;
    mDef = def;
    mDelay = speed;
    m_typeId = ENTITY_ITEM;
}

void Entity::setItem(int gfx, int hp, int mp, int atk, int def, int speed)
{
    mGfx = gfx;
    mHp = hp;
    mAtk = atk;
    mDef = def;
    mDelay = speed;
    mMp = mp;
    m_typeId = ENTITY_ITEM;
}

bool Entity::isValidPos(QPoint newPos)
{
    if(newPos.x() != mPos.x() &&
       newPos.y() != mPos.y()) {
        return true;
    } else {
        return false;
    }
}
