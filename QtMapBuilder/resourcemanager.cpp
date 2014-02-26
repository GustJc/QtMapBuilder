#include "resourcemanager.h"
#include "globals.h"
ResourceManager::ResourceManager()
{
}

Entity *ResourceManager::createEntity(std::string nome)
{
    g_entitymap.insert(nome, Entity() );

    return &g_entitymap.find(nome).value();
}

Entity *ResourceManager::getEntity(std::string nome)
{
    QMap<std::string,Entity>::iterator it =
            g_entitymap.find(nome);
    if ( it == g_entitymap.end()) return NULL;

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
