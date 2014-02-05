#ifndef LUAMANAGER_H
#define LUAMANAGER_H
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <QString>

class MapView;

class LuaManager
{
public:
    LuaManager();

    static LuaManager LuaControl;

    void startLua(MapView *mapview);

    void executeTests();
    QStringList ReportLuaError(lua_State* luaState);

    QStringList doFile(QString filename);
    QStringList loadFile(QString filename);

    void forceMapUpdate();
protected:
    luabind::scope bindClasses();
    MapView* mapView;
private:
    lua_State* L;
};

#endif // LUAMANAGER_H
