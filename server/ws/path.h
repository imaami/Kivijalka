/** \file path.h
 *
 * File/directory path object.
 */

#ifndef __PATH_H__
#define __PATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define _GNU_SOURCE

#include "list.h"

typedef struct path_head path_head_t;
typedef struct path_node path_node_t;

__attribute__((gcc_struct,packed))
struct path_head {
	list_head_t list;
	size_t      size;
	size_t      depth;
};

__attribute__((gcc_struct,packed))
struct path_node {
	list_head_t list;
	size_t      size;
	char        name[];
};

extern void
path_create (path_head_t *head,
             const char  *path);

extern char *
path_strdup (path_head_t *head);

extern void
path_destroy (path_head_t *head);

__attribute__((always_inline))
static inline size_t
path_strlen (path_head_t *head)
{
	return head->size;
}

#ifdef __cplusplus
}
#endif

#endif // __PATH_H__
