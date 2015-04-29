#include "path.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	if (argv[1]) {
		path_head_t path;
		char *str;

		path_create (&path, argv[1]);

		if ((str = path_strcpy (&path))) {
			printf ("%s (%zu)\n", str, path_strlen (&path));
			free (str);
		}

		path_destroy (&path);
	}
	return 0;
}
