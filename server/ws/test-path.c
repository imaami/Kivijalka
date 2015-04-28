#include "path.h"
#include "list.h"

#include <stdio.h>

int
main (int    argc,
      char **argv)
{
	if (argv[1]) {
		path_head_t path;
		path_init (&path);
		path_create (&path, argv[1]);

		if (!list_empty (&(path.list))) {
			printf ("path contains %u nodes\n", path.size);
			size_t i = 0;
			path_node_t *n;
			path_for_each (n, &path) {
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
