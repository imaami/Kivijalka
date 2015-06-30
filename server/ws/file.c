#include "file.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

__attribute__((always_inline))
static inline bool
file_size (FILE   *fp,
           size_t *fs)
{
	int fd = fileno (fp);
	if (fd >= 0) {
		struct stat st;
		if (!fstat (fd, &st)) {
			if (S_ISREG (st.st_mode)) {
				*fs = (st.st_size > 0)
				      ? (size_t) st.st_size
				      : 0;
				return true;
			} else {
				fprintf (stderr, "%s: not a regular file\n",
				         __func__);
			}
		} else {
			fprintf (stderr, "%s: fstat: %s\n",
			         __func__, strerror (errno));
		}
	} else {
		fprintf (stderr, "%s: fileno: %s\n",
		         __func__, strerror (errno));
	}
	return false;
}

__attribute__((always_inline))
static inline bool
file_fread (FILE    *fp,
            uint8_t *data,
            size_t  *count)
{
	for (size_t bs_left = *count, bs_read = 0, bs;; bs_left -= bs) {
		printf ("%s: trying to read %zu B to buffer offset %zu\n",
		        __func__, bs_left, bs_read);
		bs = fread ((void *) (data + bs_read), 1, bs_left, fp);
		bs_read += bs;

		if (bs != bs_left) {
			if (ferror (fp)) {
				break;
			}

			data[bs_read] = '\0';
			*count = bs_read;
			return true;
		}
	}

	return false;
}

bool
file_read (const char *path,
           size_t      size,
           uint8_t    *data,
           size_t     *count)
{
	bool r;
	size_t fs;
	FILE *fp;

	if (!path || !data || !count) {
		fprintf (stderr, "%s: invalid arguments\n", __func__);
		return false;
	}

	r = false;
	fs = 0;

	if (!(fp = fopen (path, "rb"))) {
		fprintf (stderr, "%s: fopen: %s\n",
		         __func__, strerror (errno));
	} else {
		rewind (fp);

		if (!file_size (fp, &fs)) {
			fprintf (stderr, "%s: unknown file size\n",
			         __func__);
		} else {
			printf ("%s: file size is %zu B\n", __func__, fs);

			/*
			 * if buffer is too small set *count to
			 * total required size and return false
			 */
			if (++fs > size) {
				fprintf (stderr, "%s: buffer too small\n",
				         __func__);
			} else {
				r = file_fread (fp, data, &fs);
			}
		}

		if (fclose (fp)) {
			fprintf (stderr, "%s: fclose: %s\n",
			         __func__, strerror (errno));
			r = false;
			fs = 0;
		}
		fp = NULL;
	}

	*count = fs;
	return r;
}

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
