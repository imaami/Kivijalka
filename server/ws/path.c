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

/*
	char *str1, *str2, *token, *subtoken;
	char *saveptr1, *saveptr2;
	int j;

           if (argc != 4) {
               fprintf(stderr, "Usage: %s string delim subdelim\n",
                       argv[0]);
               exit(EXIT_FAILURE);
           }

           for (j = 1, str1 = argv[1]; ; j++, str1 = NULL) {
               token = strtok_r(str1, argv[2], &saveptr1);
               if (token == NULL)
                   break;
               printf("%d: %s\n", j, token);

               for (str2 = token; ; str2 = NULL) {
                   subtoken = strtok_r(str2, argv[3], &saveptr2);
                   if (subtoken == NULL)
                       break;
                   printf(" --> %s\n", subtoken);
               }
           }

           exit(EXIT_SUCCESS);
       }
*/

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
static inline size_t
path_node_size (const size_t len)
{
	return sizeof (list_head_t) + sizeof (size_t) + len + 1;
}

__attribute__((always_inline))
static inline path_node_t *
path_node_new (const size_t  len,
               const char   *str)
{
	path_node_t *n;
	if ((n = malloc (path_node_size (len)))) {
		memcpy ((void *) n->name, (const void *) str, len);
		((char *) n->name)[len] = '\0';
		n->size = len;
	}
	return n;
}

__attribute__((always_inline))
static inline void
path_append (path_head_t  *path,
             const size_t  len,
             const char   *str)
{
	path_node_t *node;
	if ((node = path_node_new (len, str))) {
		list_add_tail (&(node->list), &(path->list));
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
inspect_path (path_head_t *head,
              const char  *path)
{
	char *p = (char *) path, *s;
	size_t b;

	printf ("path=%s\n", path);

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

	printf ("s=%s\n", s);

	for (;;) {
		switch (*p) {
		case '\0':
			path_append (head, path_substr_len (s, p), s);
			goto end;

		case '/':
			path_append (head, path_substr_len (s, p), s);
			for (; '/' == *++p;) {}
			s = p;
			continue;

		default:
			++p;
		}
	}

end:
	return;
}

int main (int argc, char **argv)
{
	if (argv[1]) {
		path_head_t path;
		list_init (&(path.list));
		path.size = 0;
		inspect_path (&path, argv[1]);
	}
	return 0;
}
