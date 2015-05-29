/** \file atomic.h
 *
 * Atomic operations.
 */

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <QtCore/QAtomicPointer>

#define atomic_update_ptr(ptr, val) {\
	typeof(val) cur;\
	do {\
		cur = (ptr).loadAcquire();\
	} while (!(ptr).testAndSetOrdered(cur, val));\
}

#ifdef __cplusplus
}
#endif

#endif // __ATOMIC_H__
