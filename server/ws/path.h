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

extern void
path_strcpy (char        *dest,
             path_head_t *head);

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

__attribute__((always_inline))
static inline void
path_copy (path_head_t       *dst,
           const path_head_t *src)
{
	dst->list.next = src->list.next;
	dst->list.prev = src->list.prev;
	dst->size = src->size;
	dst->depth = src->depth;
}

__attribute__((always_inline))
static inline bool
path_empty (path_head_t *head)
{
	return list_empty (&(head->list));
}

__attribute__((always_inline))
static inline path_node_t *
path_node (list_head_t *list_hook)
{
	return (list_hook)
	       ? list_entry (list_hook, path_node_t, list)
	       : NULL;
}

__attribute__((always_inline))
static inline path_node_t *
path_node_next (path_node_t *node)
{
	return (node)
	       ? list_next_entry (node, list)
	       : NULL;
}

__attribute__((always_inline))
static inline const char *
path_node_name (path_node_t *node)
{
	return (node) ? (const char *) node->name : NULL;
}

__attribute__((always_inline))
static inline size_t
path_node_strlen (path_node_t *node)
{
	return (node) ? node->size : 0;
}

__attribute__((always_inline))
static inline bool
path_node_is_first (path_head_t *head,
                    path_node_t *node)
{
	return (head && node && node->list.prev == &(head->list));
}

__attribute__((always_inline))
static inline bool
path_node_is_last (path_head_t *head,
                   path_node_t *node)
{
	return (head && node && node->list.next == &(head->list));
}

__attribute__((always_inline))
static inline const char *
path_filename (path_head_t *head)
{
	return (head && head->list.prev != &(head->list))
	       ? path_node_name (path_node (head->list.prev))
	       : NULL;
}

__attribute__((always_inline))
static inline size_t
path_filename_strlen (path_head_t *head)
{
	return (head && head->list.prev != &(head->list))
	       ? path_node_strlen (path_node (head->list.prev))
	       : 0;
}

#ifdef __cplusplus
}
#endif

#endif // __PATH_H__
