#include "file.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

bool
file_write (const char    *path,
            size_t         size,
            const uint8_t *data)
{
	if (path && data) {
		FILE *fp;
		if ((fp = fopen (path, "wb"))) {
			for (size_t n = 0, w; n < size; n += w) {
				w = fwrite (data + n, 1, size - n, fp);
			}
			bool r;
			if (ferror (fp)) {
				fprintf (stderr, "%s: fwrite: %s\n",
				                 __func__, strerror (errno));
				r = false;
			} else {
				r = true;
			}
			clearerr (fp);
			if (fclose (fp)) {
				fprintf (stderr, "%s: fclose: %s\n",
				                 __func__, strerror (errno));
			}
			fp = NULL;
			return r;
		} else {
			fprintf (stderr, "%s: fopen: %s\n",
			                 __func__, strerror (errno));
		}
	}
	return false;
}
