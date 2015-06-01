#include "task_buf.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	task_buf_init ();

	task_buf_fini ();

	return EXIT_SUCCESS;
}
