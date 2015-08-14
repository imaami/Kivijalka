#ifndef __KIVIJALKA_PRIVATE_CACHE_H__
#define __KIVIJALKA_PRIVATE_CACHE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../list.h"

struct cache_bucket {
	list_head_t hook;
	list_head_t list;
} __attribute__((gcc_struct,packed));

struct cache_table {
	list_head_t          in_use;
	struct cache_bucket *lookup_table;
} __attribute__((gcc_struct,packed));

#endif // __KIVIJALKA_PRIVATE_CACHE_H__

