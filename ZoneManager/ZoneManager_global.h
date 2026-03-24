#ifndef ZoneManager_GLOBAL_H
#define ZoneManager_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ZoneManager_LIBRARY)
    #define ZoneManagerSHARED_EXPORT Q_DECL_EXPORT
#else
    #define ZoneManagerSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
