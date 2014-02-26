#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H
#include <string>
class Entity;
class ResourceManager
{
public:
    ResourceManager();

    static ResourceManager ResourceControl;

    Entity* createEntity(std::string nome);
    Entity* getEntity(std::string nome);
    Entity* getEntityByIndex(int index);
};

#endif // RESOURCEMANAGER_H
