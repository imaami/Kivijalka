#include "private/parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

int
main (int    argc,
      char **argv)
{
	unsigned int i, pos;
	char *ptr;
	union {
		uint32_t u;
		int32_t  i;
	} v;
	for (i = 1; i < argc; ++i) {
		ptr = argv[i];
		for (ptr = argv[i]; *ptr; ptr += pos) {
			printf ("argv[%d]: ", i);
			pos = 0;
			if (*ptr == 'u') {
				printf ("uint32 ");
				++ptr;
				if (!_parse_u32 (ptr, &pos, &v.u)) {
					printf ("%s\n",
					        (v.u == UINT32_MAX)
					        ? "overflow"
					        : "error");
					return EXIT_FAILURE;
				}
				printf ("%u\n", v.u);
			} else {
				printf ("int32 ");
				if (!_parse_i32 (ptr, &pos, &v.i)) {
					printf ("%s\n",
					        (v.i == INT32_MAX)
					        ? "overflow"
					        : (v.i == INT32_MIN)
					          ? "underflow"
					          : "error");
					return EXIT_FAILURE;
				}
				printf ("%d\n", v.i);
			}
		}
	}
	return EXIT_SUCCESS;
}
