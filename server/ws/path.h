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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

extern void
path_create (path_head_t *head,
             const char  *path);

extern void
path_destroy (path_head_t *head);

__attribute__((always_inline))
static inline void
path_init (path_head_t *head)
{
	list_init (&(head->list));
	head->size = 0;
}

__attribute__((always_inline,pure))
static inline size_t
path_node_size (const size_t len)
{
	return sizeof (struct path_head) + len + 1;
}

__attribute__((always_inline,malloc))
static inline path_node_t *
path_node_alloc (const size_t len)
{
	path_node_t *n;
	if (!(n = malloc (path_node_size (len)))) {
		fprintf (stderr, "%s: malloc failed: %s\n", __func__, strerror (errno));
	}
	return n;
}

__attribute__((always_inline))
static inline void
path_node_fill_name (path_node_t  *node,
                     const size_t  len,
                     const char   *str)
{
	memcpy ((void *) node->name, (const void *) str, len);
	((char *) node->name)[len] = '\0';
	node->size = len;
}

__attribute__((always_inline))
static inline const char *
path_node_name (path_node_t *node)
{
	return (const char *) node->name;
}

__attribute__((always_inline))
static inline path_node_t *
path_node_new (const size_t  len,
               const char   *str)
{
	path_node_t *n;
	if ((n = path_node_alloc (len))) {
		path_node_fill_name (n, len, str);
	}
	return n;
}

__attribute__((always_inline))
static inline void
path_add_node (path_head_t  *head,
               const size_t  len,
               const char   *str)
{
	path_node_t *node;
	if (len && (node = path_node_new (len, str))) {
		list_add_tail (&(node->list), &(head->list));
		head->size++;
	}
}

__attribute__((always_inline))
static inline size_t
path_substr_len (char *start, char *end)
{
	return (size_t)((ptrdiff_t)end - (ptrdiff_t)start);
}

__attribute__((always_inline))
static inline void
path_del_node (path_head_t *head,
               path_node_t *node)
{
	list_del (&(node->list));
	head->size--;
	memset ((void *) node->name, 0, (int) node->size);
	node->size = 0;
	free (node);
}

#ifdef __cplusplus
}
#endif

#endif // __PATH_H__
