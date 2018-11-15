#ifndef DPX_GLOBAL_H
#define DPX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DPX_LIBRARY)
#  define DPXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DPXSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DPX_GLOBAL_H
