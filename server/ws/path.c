/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "path.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

__attribute__((always_inline,pure))
static inline size_t
path_node_size (const size_t len)
{
	return offsetof (struct path_node, name) + len + 1;
}

__attribute__((always_inline,malloc))
static inline path_node_t *
path_node_alloc (const size_t len)
{
	path_node_t *n;
	if (!(n = malloc (path_node_size (len)))) {
		fprintf (stderr, "%s: malloc failed: %s\n", __func__, strerror (errno));
	} else {
		n->size = len;
	}
	return n;
}

__attribute__((always_inline))
static inline void
path_node_fill_name (path_node_t  *node,
                     const size_t  len,
                     const char   *str)
{
	size_t l = (len > node->size) ? node->size : len;
	(void) memcpy ((void *) node->name, (const void *) str, l);
	node->name[l] = '\0';
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
static inline size_t
path_substr_len (char *start, char *end)
{
	return (size_t)((ptrdiff_t)end - (ptrdiff_t)start);
}

__attribute__((always_inline))
static inline void
path_init (path_head_t *head)
{
	list_init (&(head->list));
	head->size = 0;
	head->depth = 0;
}

__attribute__((always_inline))
static inline void
path_push (path_head_t  *head,
           const size_t  len,
           const char   *str)
{
	path_node_t *n;
	if (len && (n = path_node_new (len, str))) {
		size_t x = path_empty (head) ? 0 : 1;
		list_add_tail (&(n->list), &(head->list));
		head->size += (x + len);
		head->depth++;
	}
}

__attribute__((always_inline))
static inline path_node_t *
path_tail (path_head_t *head)
{
	return (!path_empty (head))
	       ? list_last_entry (&(head->list), path_node_t, list)
	       : NULL;
}

__attribute__((always_inline))
static inline path_node_t *
path_pop (path_head_t *head)
{
	path_node_t *n;
	if ((n = path_tail (head))) {
		size_t x = (n->list.prev == &(head->list)) ? 0 : 1;
		__list_del_entry (&(n->list));
		head->size -= (x + n->size);
		head->depth--;
	}
	return n;
}

__attribute__((always_inline))
static inline void
path_node_del (path_node_t *node)
{
	(void) memset ((void *) node, 0, path_node_size (node->size));
	free (node);
}

void
path_create (path_head_t *head,
             const char  *path)
{
	char *p = (char *) path, *s;

	if (!head) {
		return;
	}

	path_init (head);

	switch (*p) {
	case '\0':
		return;

	case '/':
		for (; '/' == *++p;) {
		}
		s = p-1;
		break;

	default:
		s = p++;
	}

	for (; *p;) {
		switch (*p) {
		case '/':
			path_push (head, path_substr_len (s, p), s);
			for (; '/' == *++p;) {}
			s = p;
			continue;

		default:
			++p;
		}
	}

	path_push (head, path_substr_len (s, p), s);
}

__attribute__((always_inline))
static inline void
path_write_to (path_head_t *head,
               char        *dest)
{
	char *p = dest;
	if (!path_empty (head)) {
		path_node_t *n = list_entry (head->list.next, path_node_t, list);
		(void) memcpy ((void *) p, (const void *) n->name, n->size);
		p += (ptrdiff_t) n->size;
		while (n->list.next != &(head->list)) {
			n = list_next_entry (n, list);
			*p = '/';
			++p;
			(void) memcpy ((void *) p, (const void *) n->name, n->size);
			p += (ptrdiff_t) n->size;
		}
	}
	*p = '\0';
}

void
path_strcpy (char        *dest,
             path_head_t *head)
{
	if (dest && head) {
		path_write_to (head, dest);
	}
}

char *
path_strdup (path_head_t *head)
{
	char *dest;
	if (!head) {
		dest = NULL;
	} else if ((dest = malloc (path_strlen (head) + 1))) {
		path_write_to (head, dest);
	}
	return dest;
}

void
path_destroy (path_head_t *head)
{
	if (head) {
		for (path_node_t *n;
		     (n = path_pop (head));
		     path_node_del (n));
	}
}
