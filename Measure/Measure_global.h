#ifndef MEASURE_GLOBAL_H
#define MEASURE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MEASURE_LIBRARY)
    #define MEASURESHARED_EXPORT Q_DECL_EXPORT
#else
    #define MEASURESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
