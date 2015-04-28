#include "path.h"

#include <stdio.h>

int
main (int    argc,
      char **argv)
{
	if (argv[1]) {
		path_head_t path;
		path_create (&path, argv[1]);

		if (!path_empty (&path)) {
			printf ("path contains %u nodes\n", path_depth (&path));
			size_t i = 0;
			path_node_t *n;
			path_for_each (n, &path) {
				const char *s = path_node_name (n);
				if (i) putchar ('/');
				printf ("%s", s);
				++i;
			}
			printf (" (%u)\n", path.length);
		}

		path_destroy (&path);
		if (path_empty (&path)) {
			printf ("path list emptied\n");
		}
	}
	return 0;
}
