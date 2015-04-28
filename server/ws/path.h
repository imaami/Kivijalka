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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "list.h"

typedef struct path_head path_head_t;
typedef struct path_node path_node_t;

__attribute__((gcc_struct,packed))
struct path_head {
	list_head_t list;
	size_t      size;
};

__attribute__((gcc_struct,packed))
struct path_node {
	list_head_t list;
	size_t      size;
	char        name[];
};

#define path_for_each(node, head) \
  list_for_each_entry (node, &((head)->list), list)

extern void
path_create (path_head_t *head,
             const char  *path);

extern void
path_destroy (path_head_t *head);

__attribute__((always_inline))
static inline path_node_t *
path_root (path_head_t *head)
{
	return (!list_empty (&(head->list)))
	       ? list_entry (head->list.next, path_node_t, list)
	       : NULL;
}

__attribute__((always_inline))
static inline path_node_t *
path_ascend (path_head_t *head,
             path_node_t *node)
{
	path_node_t *n = list_entry (node->list.next, path_node_t, list);
	return ((void *) n != (void *) head) ? n : NULL;
}

__attribute__((always_inline))
static inline path_node_t *
path_descend (path_head_t *head,
              path_node_t *node)
{
	path_node_t *n = list_entry (node->list.prev, path_node_t, list);
	return ((void *) n != (void *) head) ? n : NULL;
}

__attribute__((always_inline))
static inline bool
path_empty (path_head_t *head)
{
	return list_empty (&(head->list));
}

__attribute__((always_inline))
static inline size_t
path_depth (path_head_t *head)
{
	return head->size;
}

__attribute__((always_inline))
static inline const char *
path_node_name (path_node_t *node)
{
	return (const char *) node->name;
}

#ifdef __cplusplus
}
#endif

#endif // __PATH_H__
