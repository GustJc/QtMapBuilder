#include "globals.h"

QVector<Entity> g_entitylist;
QMap<std::string, Entity>    g_entitymap;
QVector<QVector<int> > g_tilesetMapTypeInfo;

bool g_showPath;
bool g_editMode;
bool g_showGrid         = true;
bool g_ShowItens        = true;
bool g_showChars        = true;
bool g_isClickActive;

bool g_selectionTool;
bool g_paintTool;
bool g_rectangleTool;
bool g_cursorTool;
bool g_startTool;
bool g_endTool;
