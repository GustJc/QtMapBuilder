#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <string>
class Entity;
class MapView;
class ResourceManager
{
public:
    ResourceManager();

    static ResourceManager ResourceControl;

    Entity* createEntity(std::string nome);
    Entity* getEntity(std::string nome);
    Entity* getEntityByIndex(int index);

    void    clearEntityMap();

    void    addGold(int px, int py, int gold);
    void    addEntityByIndex(int px, int py, int ID);
    void    addItemByIndex(int px, int py, int ID);

    void    addEntity(int px, int py, std::string name);
    void    addItem(int px, int py, std::string   name);
    void    addCustomEntity(int px, int py, Entity& ent);

    MapView* map;
};

#endif // RESOURCEMANAGER_H
