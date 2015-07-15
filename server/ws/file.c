#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "private/buf.h"

struct file {
	FILE        *fp;
	int          fd;
	struct stat  st;
	size_t       size;
	char         path[];
};

struct file *
file_create (const char *path)
{
	if (!path) {
		fprintf (stderr, "%s: null path argument\n", __func__);
		return NULL;
	}

	size_t len;

	if (1 > (len = strlen (path))) {
		fprintf (stderr, "%s: empty path argument\n", __func__);
		return NULL;
	}

	struct file *f;

	if (!(f = aligned_alloc (64, offsetof (struct file, path) + len + 1))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		return NULL;
	}

	f->fp = NULL;
	f->fd = -1;
	(void) memset ((void *) &f->st, 0, sizeof (struct stat));
	f->size = len;
	(void) memcpy ((void *) f->path, (const void *) path, len);
	f->path[len] = '\0';

	return f;
}

void
file_destroy (struct file **f)
{
	if (f) {
		struct file *_f = *f;
		if (_f) {
			size_t s = offsetof (struct file, path) + _f->size + 1;
			(void) memset ((void *) _f, 0, s);
			free (_f);
			*f = _f = NULL;
		}
		f = NULL;
	}
}

const char *
file_path (struct file *f)
{
	return (f) ? f->path : NULL;
}

__attribute__((always_inline))
static inline bool
_file_open (struct file *f,
            const char  *mode)
{
	if (!(f->fp = fopen (f->path, mode))) {
		fprintf (stderr, "%s: fopen: %s\n",
		         __func__, strerror (errno));
	} else {
		if ((f->fd = fileno (f->fp)) < 0) {
			fprintf (stderr, "%s: fileno: %s\n",
			         __func__, strerror (errno));
		} else if (fstat (f->fd, &f->st)) {
			fprintf (stderr, "%s: fstat: %s\n",
			         __func__, strerror (errno));
		} else {
			return true;
		}
		if (fclose (f->fp)) {
			fprintf (stderr, "%s: fclose: %s\n",
			         __func__, strerror (errno));
		}
		f->fp = NULL;
	}
	return false;
}

bool
file_open (struct file *f,
           const char  *mode)
{
	if (!f || !mode) {
		fprintf (stderr, "%s: null parameter(s)\n", __func__);
	} else if (f->fp) {
		fprintf (stderr, "%s: already opened\n", __func__);
	} else {
		return _file_open (f, mode);
	}
	return false;
}

__attribute__((always_inline))
static inline bool
_file_close (struct file *f)
{
	if (!fclose (f->fp)) {
		f->fp = NULL;
		f->fd = -1;
		return true;
	}
	fprintf (stderr, "%s: fclose: %s\n", __func__, strerror (errno));
	return false;
}

bool
file_close (struct file *f)
{
	if (f) {
		return _file_close (f);
	}
	fprintf (stderr, "%s: null parameter\n", __func__);
	return false;
}

__attribute__((always_inline))
static inline bool
_file_size (struct file *f,
            size_t      *size)
{
	if (S_ISREG (f->st.st_mode)) {
		*size = (f->st.st_size > 0) ? (size_t) f->st.st_size : 0;
		return true;
	}
	fprintf (stderr, "%s: not a regular file\n", __func__);
	return false;
}

bool
file_size (struct file *f,
           size_t      *size)
{
	return (f && size) ? _file_size (f, size) : false;
}

__attribute__((always_inline))
static inline bool
file_fread (FILE    *fp,
            uint8_t *data,
            size_t  *count)
{
	for (size_t bs_left = *count, bs_read = 0, bs;; bs_left -= bs) {
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

__attribute__((always_inline))
static inline bool
_file_read (struct file *f,
            size_t       size,
            uint8_t     *data,
            size_t      *count)
{
	size_t fs;

	rewind (f->fp);

	if (!_file_size (f, &fs)) {
		fprintf (stderr, "%s: unknown file size\n", __func__);
	} else if (++fs > size) {
		/*
		 * if buffer is too small set *count to
		 * total required size and return false
		 */
		*count = fs;
		fprintf (stderr, "%s: buffer too small\n", __func__);
	} else if (!file_fread (f->fp, data, &fs)) {
		fprintf (stderr, "%s: couldn't read file\n", __func__);
	} else {
		*count = fs;
		return true;
	}

	return false;
}

bool
file_read (struct file *f,
           size_t       size,
           uint8_t     *data,
           size_t      *count)
{
	if (f && data && count) {
		return _file_read (f, size, data, count);
	}
	fprintf (stderr, "%s: invalid arguments\n", __func__);
	return false;
}

__attribute__((always_inline))
static inline bool
_file_write (struct file   *f,
             size_t         size,
             const uint8_t *data)
{
	size_t n;

	if ((n = fwrite (data, 1, size, f->fp)) == size) {
		return true;
	}

	bool r;

	if (ferror (f->fp)) {
		fprintf (stderr, "%s: fwrite failed\n", __func__);
		r = false;
	} else {
		r = true;
	}

	clearerr (f->fp);

	return r;
}

bool
file_write (struct file   *f,
            size_t         size,
            const uint8_t *data)
{
	if (f && data) {
		return _file_write (f, size, data);
	}
	return false;
}

__attribute__((always_inline))
static inline bool
_file_read_to_buf (struct file *f,
                   struct buf  *buf,
                   size_t      *count)
{
	size_t fs;
	uint8_t *data;

	rewind (f->fp);

	if (!_file_size (f, &fs)) {
		fprintf (stderr, "%s: unknown file size\n", __func__);
	} else if (!(data = _buf_alloc (buf, ++fs))) {
		/*
		 * if buffer is too small set *count to
		 * total required size and return false
		 */
		*count = fs;
		fprintf (stderr, "%s: buffer too small\n", __func__);
	} else if (!file_fread (f->fp, data, &fs)) {
		fprintf (stderr, "%s: couldn't read file\n", __func__);
	} else {
		*count = fs;
		return true;
	}

	return false;
}


bool
file_read_to_buf (struct file *f,
                  struct buf  *buf,
                  size_t      *count)
{
	if (f && buf && count) {
		return _file_read_to_buf (f, buf, count);
	}
	fprintf (stderr, "%s: invalid arguments\n", __func__);
	return false;
}
