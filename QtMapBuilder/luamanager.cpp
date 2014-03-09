#include "luamanager.h"
#include <QString>
#include <QDebug>
#include <iostream>

#include "mapview.h"
#include "entity.h"
#include "resourcemanager.h"
#include "globals.h"
#include "defines.h"

LuaManager::LuaManager()
{
}
LuaManager LuaManager::LuaControl;

#include <QApplication>
#include <QTimer>
static int lua_sleep(lua_State *L)
{
    int m = static_cast<int> (luaL_checknumber(L,1));
    QTimer timer;

    timer.start(m);
    while(timer.remainingTime() > 0)
        qApp->processEvents();


    LuaManager::LuaControl.forceMapUpdate();

    return 0;
}

void LuaManager::startLua(MapView* mapview)
{
    this->mapView = mapview;
    L = luaL_newstate();
    luaL_openlibs(L);
    luabind::open(L);

    luabind::module(L) [
        bindClasses()
    ];

    luabind::globals(L)["map"] =
        mapView;

    luabind::globals(L)["res"] =
        &ResourceManager::ResourceControl;

    lua_pushcfunction(L, lua_sleep);
    lua_setglobal(L, "sleep");

    //executeTests();

    //doFile("./data/scripts/itens.lua");
    //doFile("./data/scripts/entitys.lua");

}

void LuaManager::executeTests()
{
    luaL_dostring(
        L,
        "print('Print funcionando')\n"
        "t = Tile()\n"
        "t.x = 10\n"
        "print(t.x)\n"
        "map:createMap(20,20)\n"
                "sleep(250)\n"
        "map:setTile(5,5, 5, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(6,5, 5, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(7,5, 5, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(8,5, 0, Tile.PASS)\n"

                "sleep(250)\n"

        "map:setTile(5,6, 1, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(5,7, 1, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(5,8, 1, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(6,9, 1, Tile.PASS)\n"

                "sleep(250)\n"

        "map:setTile(7,9, 3, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(8,9, 3, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(9,8, 3, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(8,7, 3, Tile.PASS)\n"
                "sleep(250)\n"
        "map:setTile(7,7, 4, Tile.PASS)\n"

                "sleep(250)\n"
      "\n"
    );


    ReportLuaError(L);
}

luabind::scope LuaManager::bindClasses()
{
    return
        luabind::class_<MapView>("Map")
        .def( luabind::constructor<>( ) )
       .property("h", &MapView::getMapHeight)
       .property("w", &MapView::getMapWidth)
       .def("createMap", &MapView::newMapInt)
       .def("getTile", &MapView::getTile)
       .def("setTile", &MapView::setTile),

    luabind::class_<Tile>("Tile")
        .def( luabind::constructor<>( ) )
        .def_readwrite("gfx", &Tile::gfx)
        .def_readwrite("type", &Tile::type)
        .enum_("Tile_type")
        [
            luabind::value("NONE", TYPE_NONE),
            luabind::value("BLOCK", TYPE_BLOCK),
            luabind::value("PASS", TYPE_PASS),
            luabind::value("START", TYPE_START),
            luabind::value("END", TYPE_END)
        ],

    luabind::class_<Entity>("Entity")
        .def( luabind::constructor<>( ) )
        .def_readwrite("mHp", &Entity::mHp)
        .def_readwrite("mMp", &Entity::mMp)
        .def_readwrite("mSpeedCost", &Entity::mDelay)
        .def_readwrite("mRange", &Entity::mRange)
        .def_readwrite("mAtk", &Entity::mAtk)
        .def_readwrite("mDef", &Entity::mDef)
        .def("setEnemy", &Entity::setEnemy)
        .def("setGold", &Entity::setGold)
        .def("setItemRecover", &Entity::setItemRecover)
        .def("setItemStatus", &Entity::setItemStatus)
        .def("setItem", &Entity::setItem),

    luabind::class_<ResourceManager>("ResourceManager")
        .def( luabind::constructor<>( ) )
        .def( "createEntity", &ResourceManager::createEntity)
        .def( "getEntity", &ResourceManager::getEntity)
        .def( "getEntityByIndex", &ResourceManager::getEntityByIndex)
            .def( "addGold", &ResourceManager::addGold)
        .def( "addEntityByIndex", &ResourceManager::addEntityByIndex)
        .def( "addItemByIndex", &ResourceManager::addItemByIndex)
        .def( "addEntity", &ResourceManager::addEntity)
        .def( "addItem", &ResourceManager::addItem)
        .def( "addCustomEntity", &ResourceManager::addCustomEntity)
        .def( "clearEntityMap", &ResourceManager::clearEntityMap),


    luabind::class_<std::string>("stdString")
        .def( luabind::constructor<>( ) )
    ;
}

QStringList LuaManager::ReportLuaError(lua_State *luaState)
{
    const char* msg = lua_tostring(luaState,-1);
    QStringList strList;
    while(msg)
    {
        lua_pop(luaState,1);
        std::cout << msg << std::endl;
        qDebug() << msg;
        strList << msg;
        msg = lua_tostring(L,-1);
    }

    return strList;
}

QStringList LuaManager::doFile(QString filename)
{
    QStringList strList;
    if( luaL_dofile(L, filename.toStdString().c_str()) ) {
        strList = ReportLuaError(L);
    }

    return strList;
}

QStringList LuaManager::loadFile(QString filename)
{
    QStringList strList;
    if( luaL_loadfile(L, filename.toStdString().c_str()) ) {
         strList = ReportLuaError(L);
    }

    return strList;
}

void LuaManager::forceMapUpdate()
{
    mapView->forceUpdateMap();
}

