#include "resourcemanager.h"
#include "globals.h"
#include "mapview.h"
#include "qdebug.h"
ResourceManager ResourceManager::ResourceControl;
ResourceManager::ResourceManager()
{
}

Entity *ResourceManager::createEntity(std::string nome)
{
    g_entitymap.insert(nome, Entity() );

    return &g_entitymap.find(nome).value();
}

Entity *ResourceManager::createItem(std::string nome)
{
    g_itemmap.insert(nome, Entity() );

    return &g_itemmap.find(nome).value();
}

Entity *ResourceManager::getEntity(std::string nome)
{
    QMap<std::string,Entity>::iterator it =
            g_entitymap.find(nome);
    if ( it == g_entitymap.end()) return NULL;

    return &it.value();
}

Entity *ResourceManager::getItem(std::string nome)
{
    QMap<std::string,Entity>::iterator it =
            g_itemmap.find(nome);
    if ( it == g_itemmap.end()) return NULL;

    return &it.value();
}

Entity *ResourceManager::getEntityByIndex(int index)
{
    QMap<std::string,Entity>::iterator any;
    int i = 0;
    for (any = g_entitymap.begin(); any != g_entitymap.end(); ++any){
        if(i == index)
            return &any.value();

        i++;
    }

    return NULL;
}

Entity *ResourceManager::getItemByIndex(int index)
{
    QMap<std::string,Entity>::iterator any;
    int i = 0;
    for (any = g_itemmap.begin(); any != g_itemmap.end(); ++any){
        if(i == index)
            return &any.value();

        i++;
    }

    return NULL;
}

void ResourceManager::clearEntityMap()
{
    g_entitymap.clear();
}

void ResourceManager::clearItemMap()
{
    g_itemmap.clear();
}

void ResourceManager::addGold(int px, int py, int gold)
{
    int goldSize = gold/5;
    if(goldSize > 5) goldSize = 5;

    if(goldSize != 0){
        goldSize = gold/(5*goldSize);
    }
    if(goldSize > 5) {
        goldSize = 5;
    }

    Entity item;
    item.setGold(gold);
    item.mGfx = goldSize;
    map->setEntity(px, py, item);
}

void ResourceManager::addEntityByIndex(int px, int py, int ID)
{
    Entity* ent = getEntityByIndex(ID);

    if( ent != NULL) {
        map->setEntity(px, py, *ent);
    }
}

void ResourceManager::addItemByIndex(int px, int py, int ID)
{
    Entity* ent = getItemByIndex(ID);

    if( ent != NULL) {
        map->setEntity(px, py, *ent);
    }
}

void ResourceManager::addEntity(int px, int py, std::string name)
{
    Entity* ent = getEntity(name);

    if( ent != NULL) {
        map->setEntity(px, py, *ent);
    }
}

void ResourceManager::addItem(int px, int py, std::string name)
{
    Entity* ent = getItem(name);

    if( ent != NULL) {
        map->setEntity(px, py, *ent);
    }
}

void ResourceManager::addCustomEntity(int px, int py, Entity &ent)
{
    map->setEntity(px, py, ent);
}



