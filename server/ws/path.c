#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/time.h>
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
path_append (path_head_t  *head,
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
path_create (path_head_t *head,
             const char  *path)
{
	char *p = (char *) path, *s;

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

	for (;;) {
		switch (*p) {
		case '\0':
			path_append (head, path_substr_len (s, p), s);
			return;

		case '/':
			path_append (head, path_substr_len (s, p), s);
			for (; '/' == *++p;) {}
			s = p;
			continue;

		default:
			++p;
		}
	}
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

__attribute__((always_inline))
static inline void
path_destroy (path_head_t *head)
{
	path_node_t *n, *n2;
	list_for_each_entry_safe_reverse (n, n2, &(head->list), list) {
		path_del_node (head, n);
	}
}

int main (int argc, char **argv)
{
	if (argv[1]) {
		path_head_t path;
		path_init (&path);
		path_create (&path, argv[1]);

		if (!list_empty (&(path.list))) {
			printf ("path contains %u nodes\n", path.size);
			size_t i = 0;
			path_node_t *n;
			list_for_each_entry (n, &(path.list), list) {
				const char *s = path_node_name (n);
				if (i) putchar ('/');
				printf ("%s", s);
				++i;
			}
			puts ("");
		}

		path_destroy (&path);
		if (list_empty (&(path.list))) {
			printf ("path list emptied\n");
		}
	}
	return 0;
}
