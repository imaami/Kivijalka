#ifndef __KIVIJALKA_PRIVATE_CACHE_H__
#define __KIVIJALKA_PRIVATE_CACHE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../list.h"
#include "hex.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

struct cache_bucket {
	list_head_t hook;
	list_head_t list;
} __attribute__((gcc_struct,packed));

struct cache_table {
	list_head_t          in_use;
	struct cache_bucket *lookup_table;
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline bool
_cache_mkdir (char          *path,
              size_t         pos,
              const size_t   id_len,
              const uint8_t *id)
{
	size_t k, l;
	unsigned int i;
	struct stat sb;
	int e;

	i = id[0];
	l = pos;
	path[l++] = _hex_char (i >> 4);
	path[l++] = _hex_char (i & 0x0f);
	path[l] = '/';

	k = l + 1;
	for (size_t j = 1; j < id_len; ++j) {
		i = id[j];
		path[k++] = _hex_char (i >> 4);
		path[k++] = _hex_char (i & 0x0f);
	}
	path[k] = '\0';

	printf ("%s: target: %s\n", __func__, path);
	path[l] = '\0';

	/* check for, and if necessary create, first part of path */
	if (stat (path, &sb) == -1) {
		if ((e = errno) != ENOENT) {
			goto _fail_stat;
		}

		printf ("%s: trying to mkdir: %s\n", __func__, path);
		errno = 0;

		if (mkdir (path, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) == -1) {
			goto _fail_mkdir;
		}

		printf ("%s: created path: %s\n", __func__, path);
		path[l] = '/';
		goto _create_subdir;

	} else if (!S_ISDIR(sb.st_mode)) {
		goto _fail_notadir;

	} else {
		printf ("%s: path exists: %s\n", __func__, path);
	}

	/* check for, and if necessary create, remaining part of path */
	path[l] = '/';
	if (stat (path, &sb) == -1) {
		if ((e = errno) != ENOENT) {
		_fail_stat:
			fprintf (stderr, "%s: stat: %s\n", __func__,
			         strerror (e));
			return false;
		}

	_create_subdir:
		printf ("%s: trying to mkdir: %s\n", __func__, path);
		errno = 0;

		if (mkdir (path, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) == -1) {
		_fail_mkdir:
			fprintf (stderr, "%s: mkdir: %s\n", __func__,
			         strerror (errno));
			return false;
		}

		printf ("%s: created path: %s\n", __func__, path);

	} else if (!S_ISDIR(sb.st_mode)) {
	_fail_notadir:
		fprintf (stderr, "%s: not a directory: %s\n", __func__, path);
		return false;

	} else {
		printf ("%s: path exists: %s\n", __func__, path);
	}

	return true;
}

#endif // __KIVIJALKA_PRIVATE_CACHE_H__

