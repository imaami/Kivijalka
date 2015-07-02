#include "buf.h"

int
main (int    argc,
      char **argv)
{
	buf_t *b;

	if (!(b = buf_create (1920*1080*4))) {
		fprintf (stderr, "%s: buf_create failed\n", __func__);
		return EXIT_FAILURE;
	}

	printf ("%s: created buffer; %zu B free space\n"
	        "%s: current pointer: %p\n",
	        __func__, b->size, __func__, (char *) b->data);

	char *p = buf_alloc (b, 1920*4);

	printf ("%s: allocated %zu B\n"
	        "%s: pointer is now %p\n",
	        __func__, b->used, __func__, (char *) (b->data + b->used));

	p = buf_alloc (b, 1920*1080*4);
	printf ("%s: purposefully attempted excessive allocation,"
	        " returned pointer was %p\n", __func__, p);

	buf_destroy (&b);
	b = NULL;

	printf ("%s: destroyed buffer\n", __func__);

	return EXIT_SUCCESS;
}
