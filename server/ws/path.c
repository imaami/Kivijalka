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

typedef struct list_node list_node_t;
typedef struct path_head path_head_t;
typedef struct path_node path_node_t;

__attribute__((gcc_struct,packed))
struct list_node {
	union {
		list_node_t *next;
		list_node_t *head;
	};
	union {
		list_node_t *prev;
		list_node_t *tail;
	};
};

__attribute__((always_inline))
static inline void
list_init (list_node_t *list)
{
	list->next = list;
	list->prev = list;
}

__attribute__((gcc_struct,packed))
struct path_head {
	list_node_t list;
	size_t      size;
};

__attribute__((gcc_struct,packed))
struct path_node {
	list_node_t list;
	size_t      size;
	char        name[];
};

__attribute__((always_inline))
static inline path_node_t *
path_node_new (const size_t  size,
               const char   *name)
{
	path_node_t *n;
	if ((n = malloc ((2 * sizeof (path_node_t *))
	                 + sizeof (size_t) + size + 1))) {
		list_init (&n->list);
		n->size = size;
		memcpy ((void *) n->name, (const void *) name, size);
		((char *) n->name)[size] = '\0';
	}
	return n;
}

__attribute__((always_inline))
static inline size_t
print_substr (char *start, char *end)
{
	size_t b = (size_t)((ptrdiff_t)end - (ptrdiff_t)start);
	for (char *p = start; p < end; ++p) {
		putchar (*p);
	}
	if (b > 0) printf (" (%lu)\n", b);
	return b;
}

__attribute__((always_inline))
static inline void
inspect_path (path_head_t *head,
              const char  *path)
{
	char *p = (char *) path, *s;
	size_t b;
	path_node_t *n;

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
			b = print_substr (s, p);
//			b = (size_t)((ptrdiff_t)p - (ptrdiff_t)s);
			goto end;

		case '/':
			b = print_substr (s, p);
//			b = (size_t)((ptrdiff_t)p - (ptrdiff_t)s);
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
		path.list.head = &path.list;
		path.list.tail = &path.list;
		path.size = 0;
		inspect_path (&path, argv[1]);
	}
	return 0;
}
