#ifndef LYXYHC_GLOBAL_H
#define LYXYHC_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LYXYHC_LIBRARY)
    #define LYXYHCSHARED_EXPORT Q_DECL_EXPORT
#else
    #define LYXYHCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif
