#ifndef DBC_COMPAT_H
#define DBC_COMPAT_H

#if !defined(STD_NULLPTR_T) || !STD_NULLPTR_T
#ifndef nullptr
#include <stddef.h>
#define nullptr NULL
#endif
#endif

#endif
