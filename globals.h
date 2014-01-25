#ifndef GLOBALS_H
#define GLOBALS_H

#include <QVector>
#include "entity.h"

#define START_TOOL 1
#define END_TOOL   2

extern QVector<Entity> g_entitylist;
extern bool g_showPath;
extern bool g_editMode;
extern bool g_showGrid;
extern bool g_ShowEntities;
extern bool g_isClickActive;

extern bool g_paintTool;
extern bool g_rectangleTool;
extern bool g_cursorTool;
extern bool g_startTool;
extern bool g_endTool;

#endif // GLOBALS_H
