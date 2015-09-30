#ifndef __KIVIJALKA_PRIVATE_CACHE_H__
#define __KIVIJALKA_PRIVATE_CACHE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef _DIRENT_HAVE_D_TYPE
#error The dirent structure does not have a d_type field on this platform
#endif

#include "mem.h"
#include "hex.h"
#include "img.h"
#include "sha1.h"
#include "json.h"
#include "parse.h"
#include "banner.h"
#include "display.h"
#include "../list.h"
#include "../cache.h"
#include "../global.h"
#include "../img_data.h"
#include "../img_file.h"

typedef struct cache cache_t;
typedef struct cache_bucket cache_bucket_t;

struct cache_bucket {
	list_head_t hook;
	list_head_t list;
} __attribute__((gcc_struct,packed));

struct cache {
	struct cache_bucket data[2][256];
	list_head_t         used[2];
	size_t              path_len;
	char                path[];
} __attribute__((gcc_struct,packed));

struct cache_packet {
	uint64_t time;
	uint32_t inum;
	uint32_t bnum;
	uint32_t uuid[4];
	uint8_t  data[];
} __attribute__((gcc_struct,packed));

enum {
	CACHE_UUID = 0,
	CACHE_HASH = 1
};

__attribute__((always_inline))
static inline struct cache *
_cache_create (const char *path)
{
	struct cache *c;
	union {
		char        *c;
		list_head_t *l;
	} dest;
	const char *src;
	size_t path_len, i, k, alloc_size;

	if (path) {
		if (!path[0]) {
			goto _have_no_path;
		}

		k = 0;
		path_len = 0;
		goto _path_count_loop;

		for (; path[path_len]; ++path_len) {
		_path_count_loop:
			if (path[path_len] != '/') {
				continue;
			}

			for (; path[++path_len] == '/'; ++k);

			if (!path[path_len]) {
				goto _have_trailing_slash;
			}
		}

		++path_len;

	_have_trailing_slash:
		src = path;
		path_len -= k;

	} else {
	_have_no_path:
		src = NULL;
		path_len = 2;
	}

	k = offsetof (struct cache, path);

	if (!(c = _mem_new (6, k + path_len + 1, &alloc_size))) {
		fprintf (stderr, "%s: allocation failed\n",
		         __func__);
		return NULL;
	}

	printf ("%s: allocated %zu B\n", __func__, alloc_size);

	dest.l = (list_head_t *) c;
	for (i = 0; i < 1025; ++i) {
		list_init (dest.l++);
	}
	list_init (dest.l);

	c->path_len = path_len;

	dest.c = (char *) c;

	if (src) {
		for (i = 0; src[i]; ++i) {
		_path_write_loop:
			dest.c[k++] = src[i];

			if (src[i] != '/') {
				continue;
			}

			for (; src[++i] == '/';);

			if (!src[i]) {
				goto _end;
			}

			goto _path_write_loop;
		}
	} else {
		dest.c[k++] = '.';
	}

	dest.c[k++] = '/';

_end:
	while (k < alloc_size) {
		dest.c[k++] = '\0';
	}

	return c;
}

__attribute__((always_inline))
static inline void
_cache_destroy (struct cache *c)
{
	free (c);
}

__attribute__((always_inline))
static inline const char *
_cache_path (struct cache *c)
{
	return (const char *) c->path;
}

__attribute__((always_inline))
static inline size_t
_cache_path_length (struct cache *c)
{
	return c->path_len;
}

__attribute__((always_inline))
static inline bool
_cache_banner_path (struct cache  *c,
                    struct banner *b,
                    char          **path,
                    size_t         *len)
{
	size_t l, n, k;
	char *p;
	unsigned int i;

	l = _cache_path_length (c);

	if (!(p = _mem_new (3, l + 36, &n))) {
		return false;
	}

	strncpy (p, _cache_path (c), l);

	i = b->uuid[0];
	p[l++] = _hex_char (i >> 4);
	p[l++] = _hex_char (i & 0x0f);
	p[l++] = '/';

	for (k = 1; k < 16; ++k) {
		i = b->uuid[k];
		p[l++] = _hex_char (i >> 4);
		p[l++] = _hex_char (i & 0x0f);
	}

	p[l++] = '/';

	for (k = l; k < n; ++k) {
		p[k] = '\0';
	}

	*path = p;
	*len = l;

	return true;
}

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

__attribute__((always_inline))
static inline struct cache_bucket *
_cache_bucket (struct cache *c,
               int           type,
               uint8_t       idx)
{
	return &c->data[type][idx];
}

__attribute__((always_inline))
static inline struct cache_bucket *
_cache_bucket_by_uuid (struct cache *c,
                       uuid_t        uuid)
{
	return _cache_bucket (c, CACHE_UUID, uuid[0]);
}

__attribute__((always_inline))
static inline struct cache_bucket *
_cache_bucket_by_hash (struct cache *c,
                       sha1_t        hash)
{
	return _cache_bucket (c, CACHE_HASH, hash.u8[0]);
}

__attribute__((always_inline))
static inline struct img *
_cache_find_img_by_hash (struct cache *c,
                         sha1_t        hash)
{
	struct cache_bucket *b = _cache_bucket_by_hash (c, hash);
	return _img_find_in_list_by_hash (&b->list, hash);
}

__attribute__((always_inline))
static inline struct banner *
_cache_find_banner_by_uuid (struct cache *c,
                            uuid_t        uuid)
{
	struct cache_bucket *b = _cache_bucket_by_uuid (c, uuid);
	return _banner_find_in_list_by_uuid (&b->list, uuid);
}

__attribute__((always_inline))
static inline struct banner *
_cache_find_banner_by_uuid_str (struct cache *c,
                                const char   *uuid_str)
{
	struct banner *b;
	uuid_t uuid;
	size_t n;

	if (_parse_uuid (uuid_str, uuid, &n)) {
		b = _cache_find_banner_by_uuid (c, uuid);

	} else {
		fprintf (stderr, "%s: invalid UUID string: syntax error at "
		         "offset %zu\n", __func__, n);
		b = NULL;
	}

	return b;
}

__attribute__((always_inline))
static inline struct img *
_cache_find_or_add_img (struct cache  *c,
                        sha1_t         hash)
{
	struct cache_bucket *b = _cache_bucket_by_hash (c, hash);
	struct img *im = _img_find_in_list_by_hash (&b->list, hash);

	if (!im) {
		if ((im = _img_create (&b->list, &hash, NULL,
		                       0, NULL, 0, 0, NULL))) {
			if (b->hook.next == &b->hook) {
				list_add (&b->hook, &c->used[CACHE_HASH]);
			}
		}
	}

	return im;
}

__attribute__((always_inline))
static inline struct img *
_cache_find_or_add_img_from_packet (struct cache         *c,
                                    struct banner_packet *p)
{
	struct cache_bucket *b = _cache_bucket_by_hash (c, p->hash);
	list_head_t *list = &b->list;
	struct img *im = _img_find_in_list_by_hash (list, p->hash);

	if (!im) {
		printf ("%s: received image not previously cached, trying to "
		        "add it", __func__);

		size_t fsiz, size;
		struct {
			union {
				uint8_t *fname;
				char    *fpath;
			};
			union {
				uint8_t       *data;
				const uint8_t *hash;
			};
		} ptr_to;

		fsiz = p->fsiz;
		size = p->size;
		ptr_to.fname = p->data + p->nsiz;
		ptr_to.data = ptr_to.fname + fsiz;

		if (!(im = _img_create_from_packet (list, &p->hash, fsiz, size,
		                                    ptr_to.fname, ptr_to.data))) {
			fprintf (stderr,
			         "%s: failed to extract packet payload\n",
			         __func__);
		} else {
			printf ("%s: added image to cache\n", __func__);

			list = &b->hook;
			if (list->next == list) {
				list_add (list, &c->used[CACHE_HASH]);
			}

			size_t root_len = _cache_path_length (c);
			fsiz = _img_name_length (im);

			if (!(ptr_to.fpath = _mem_new (3, root_len + 43 + fsiz,
			                               &size))) {
				return false;
			}

			strncpy (ptr_to.fpath, c->path, root_len);
			ptr_to.fpath[root_len] = '\0';

			ptr_to.hash = ((const uint8_t *) im)
			              + offsetof (struct img, hash);

			if (_cache_mkdir (ptr_to.fpath, root_len, 20,
			                  ptr_to.hash)) {
				ptr_to.fpath[root_len + 41] = '/';
				ptr_to.fpath[root_len + 42] = '\0';
				_img_export (im, ptr_to.fpath, root_len + 42);
			}

			_mem_del (&ptr_to.fpath);
		}
	} else {
		printf ("%s: received image has been previously cached with "
		        "filename \"%s\", using it for this upload", __func__,
		        _img_name (im));
	}

	return im;
}

__attribute__((always_inline))
static inline bool
_cache_add_banner (struct cache  *c,
                   struct banner *b,
                   struct img    *im)
{
	uuid_t uuid;
	struct cache_bucket *cb;

	// link banner into image's list of banner references
	list_add (&b->by_hash, &im->refs);

	do {
		uuid_generate (uuid);
		cb = _cache_bucket_by_uuid (c, uuid);
	} while (_banner_find_in_list_by_uuid (&cb->list, uuid));

	uuid_copy (b->uuid, uuid);

	// link banner into uuid-indexed table
	list_add (&b->by_uuid, &cb->list);
	if (cb->hook.next == &cb->hook) {
		list_add (&cb->hook, &c->used[CACHE_UUID]);
	}

	return true;
}

__attribute__((always_inline))
static inline bool
_cache_export_banner (struct cache  *c,
                      struct banner *b)
{
	const char *root_path = (const char *) c->path;
	size_t path_len = _cache_path_length (c), path_alloc;
	char *path;
	const uint8_t *id;
	bool r;

	if ((path = _mem_new (6, path_len + 36, &path_alloc))) {
		strncpy (path, root_path, path_len);
		path[path_len] = '\0';

		id = ((const uint8_t *)b) + offsetof (struct banner, uuid);

		if (_cache_mkdir (path, path_len, 16, id)) {
			path[path_len + 33] = '/';
			path[path_len + 34] = '\0';
			r = _banner_export (b, path, path_len + 34);

		} else {
			fprintf (stderr, "%s: failed to create export path\n",
			         __func__);
			r = false;
		}

		_mem_del (&path);

	} else {
		fprintf (stderr, "%s: failed to allocate path string\n",
		         __func__);
		r = false;
	}

	return r;
}

__attribute__((always_inline))
static inline struct img *
_cache_add_img_from_disk (struct cache *c,
                          const char   *path,
                          sha1_t        expect_hash)
{
	bool r;
	uint8_t *data;
	size_t size;
	sha1_t hash;
	struct img *im;
	struct cache_bucket *b;
	char *name;

	if (!_read_img_file (path, &data, &size)) {
		fprintf (stderr, "%s: _read_img_file failed\n", __func__);
		return NULL;
	}

	_sha1_gen (&hash, size, data);

	if (!_sha1_cmp (&hash, &expect_hash)) {
		_sha1_init (&hash);
		fprintf (stderr, "%s: file hash differs from expected value\n",
		         __func__);
		goto _fail_free_data;
	}

	b = _cache_bucket_by_hash (c, hash);

	if ((im = _img_find_in_list_by_hash (&b->list, hash))) {
		if ((im->data)) {
			if (im->size == size && im->data[0] == data[0]) {
				/* most likely the same piece of data is already
				 * present in the cache - do nothing but succeed
				 */
				r = true;
			} else {
				fprintf (stderr, "%s: hash collision\n", __func__);
				r = false;
			}
		} else {
			/* this image was added previously without
			 * image file payload, let's add it now
			 */
			im->size = size;
			im->data = data;
			if ((name = _img_name_from_path (path, &size))) {
				_img_set_name (im, name, size);
			}
			goto _succeed;
		}
	} else {
		if ((im = _img_create (&b->list, &hash, path,
		                       size, data, 0, 0, NULL))) {
			if (b->hook.next == &b->hook) {
				list_add (&b->hook, &c->used[CACHE_HASH]);
			}
		_succeed:
			data = NULL;
			r = true;
			goto _end;
		} else {
			fprintf (stderr, "%s: _img_create failed\n", __func__);
		_fail_free_data:
			r = false;
		}
	}

	(void) memset ((void *) data, 0, size);
	_mem_del (&data);

_end:
	size = 0;
	return r ? im : NULL;
}

__attribute__((always_inline))
static inline bool
_textfile_read (const char *path,
                uint8_t    *dest,
                size_t      size,
                size_t     *len)
{
	bool r;
	file_t *f;
	size_t l;

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		return false;
	}

	if (!file_open (f, "r")) {
		fprintf (stderr, "%s: file_open failed\n", __func__);
		r = false;
		goto destroy_file;
	}

	if (!file_read (f, size, dest, &l)) {
		fprintf (stderr, "%s: file_read failed\n", __func__);
		r = false;
		goto close_file;
	}

	dest[l] = '\0';
	*len = l;
	r = true;

close_file:
	if (!file_close (f)) {
		fprintf (stderr, "%s: file_close failed\n", __func__);
	}

destroy_file:
	file_destroy (&f);

	return r;
}

__attribute__((always_inline))
static inline void
_cache_import_banner (struct cache  *cache,
                      char          *path,
                      size_t         path_pos,
                      struct dirent *entry,
                      uuid_t         uuid)
{
	DIR *dirp;

	if (!(dirp = opendir (path))) {
		fprintf (stderr, "%s: opendir failed: %s\n", __func__,
		         strerror (errno));
		return;
	}

	uint8_t buf[256];
	size_t size, name_len = 0;
	sha1_t hash = SHA1_INITIALIZER;
	uint32_t w = 0, h = 0;
	int32_t x = 0, y = 0;
	char *banner_name = NULL;

	for (int i;;) {
		struct dirent *result;
		if ((i = readdir_r (dirp, entry, &result))) {
			fprintf (stderr,
			         "%s: readdir_r failed: %s\n",
			         __func__, strerror (i));
			result = NULL;
			i = 0;
			goto close_dir;
		}

		if (!result) {
			// end of directory stream
			break;
		}

		if (result->d_type != DT_REG) {
			// these are not the entries you're looking for
			continue;
		}

		const char *name = (const char *) result->d_name;

		switch (name[0]) {
		case 'h':
			if (!name[1]) {
				path[path_pos] = 'h';
				path[path_pos + 1] = '\0';
				size = 0;
				if (_textfile_read (path, buf, 256, &size)) {
					unsigned int j = 0;
					if (!_parse_u32 ((char *) buf, &j, &h)) {
						h = 0;
					}
				}
				path[path_pos] = '\0';
			}
			continue;

		case 'i':
			if (!name[1]) {
				path[path_pos] = 'i';
				path[path_pos + 1] = '\0';
				size = 0;
				if (_textfile_read (path, buf, 256, &size)) {
					if (!_sha1_parse ((const char *) buf, &hash)) {
						_sha1_init (&hash);
					}
				}
				path[path_pos] = '\0';
			}
			continue;

		case 'n':
			if (!name[1]) {
				path[path_pos] = 'n';
				path[path_pos + 1] = '\0';
				size = 0;
				if (_textfile_read (path, buf, 256, &size)) {
					if (!_banner_name_dup ((const char *) buf,
					                       size, &banner_name,
					                       &name_len)) {
						banner_name = NULL;
						name_len = 0;
					}
				}
				path[path_pos] = '\0';
			}
			continue;

		case 'w':
			if (!name[1]) {
				path[path_pos] = 'w';
				path[path_pos + 1] = '\0';
				size = 0;
				if (_textfile_read (path, buf, 256, &size)) {
					unsigned int j = 0;
					if (!_parse_u32 ((char *) buf, &j, &w)) {
						w = 0;
					}
				}
				path[path_pos] = '\0';
			}
			continue;

		case 'x':
			if (!name[1]) {
				path[path_pos] = 'x';
				path[path_pos + 1] = '\0';
				size = 0;
				if (_textfile_read (path, buf, 256, &size)) {
					unsigned int j = 0;
					if (!_parse_i32 ((char *) buf, &j, &x)) {
						x = 0;
					}
				}
				path[path_pos] = '\0';
			}
			continue;

		case 'y':
			if (!name[1]) {
				path[path_pos] = 'y';
				path[path_pos + 1] = '\0';
				size = 0;
				if (_textfile_read (path, buf, 256, &size)) {
					unsigned int j = 0;
					if (!_parse_i32 ((char *) buf, &j, &y)) {
						y = 0;
					}
				}
				path[path_pos] = '\0';
			}
		}
	}

	struct cache_bucket *cb = _cache_bucket_by_uuid (cache, uuid);
	struct banner *b = _banner_find_in_list_by_uuid (&cb->list, uuid);
	struct img *im;

	if (!b) {
		if ((b = _banner_create ())) {
			uuid_copy (b->uuid, uuid);
			b->name_len = name_len;
			b->name = banner_name;
			b->offset.x = x;
			b->offset.y = y;
			b->dims.w = w;
			b->dims.h = h;
			_sha1_cpy (&hash, &b->hash);

			uuid_unparse_lower (uuid, (char *) buf);

			if ((im = _cache_find_or_add_img (cache, hash))) {
				list_add (&b->by_hash, &im->refs);
				if (im->name) {
					if (b->name) {
						printf ("%s: \"%s\" -> \"%s\"\n",
						        __func__, im->name, b->name);
					} else {
						printf ("%s: \"%s\" -> %s\n",
						        __func__, im->name, buf);
					}
				} else {
					_sha1_str (&hash, (char *) buf + 37);
					if (b->name) {
						printf ("%s: %s -> \"%s\"\n",
						        __func__, buf + 37, b->name);
					} else {
						printf ("%s: %s -> %s\n",
						        __func__, buf + 37, buf);
					}
				}
			}

			list_add (&b->by_uuid, &cb->list);
			if (cb->hook.next == &cb->hook) {
				list_add (&cb->hook, &cache->used[CACHE_UUID]);
			}

			printf ("%s: imported: %s\n", __func__, buf);
			goto _succeed;
		} else {
			fprintf (stderr, "%s: failed to create banner\n",
			         __func__);
		}
	} else {
		fprintf (stderr, "%s: banner already loaded\n", __func__);
	}

close_dir:
	if (banner_name) {
		free (banner_name);
	_succeed:
		banner_name = NULL;
	}

	if (closedir (dirp) == -1) {
		fprintf (stderr, "%s: closedir failed: %s\n",
		         __func__, strerror (errno));
	}

	dirp = NULL;
}

__attribute__((always_inline))
static inline void
_cache_import_img (struct cache  *cache,
                   char          *path,
                   size_t         path_pos,
                   struct dirent *entry,
                   sha1_t         hash)
{
	DIR *dirp;
	struct img *im;

	if ((dirp = opendir (path))) {
		for (int i;;) {
			struct dirent *result;
			if ((i = readdir_r (dirp, entry, &result))) {
				fprintf (stderr,
				         "%s: readdir_r failed: %s\n",
				         __func__, strerror (i));
				result = NULL;
				i = 0;
				goto close_dir;
			}

			if (!result) {
				// end of directory stream
				break;
			}

			const char *name;

			switch (result->d_type) {
			case DT_REG:
				name = (const char *) result->d_name;
				unsigned int k;

				if (name[0] == 'i' && name[1] == '\0') {
					path[path_pos] = 'i';
					path[path_pos + 1] = '\0';
					goto _read_file;
				}

				for (k = 0; name[k]; ++k) {
					path[path_pos + k] = name[k];
					switch (name[k]) {
					case '.':
						++k;
						path[path_pos + k] = name[k];
						switch (name[k]) {
						case '\0':
							goto _skip;
						case 'G': case 'g':
							++k;
							path[path_pos + k] = name[k];
							switch (name[k]) {
							case '\0':
								goto _skip;
							case 'I': case 'i':
								++k;
								path[path_pos + k] = name[k];
								switch (name[k]) {
								case '\0':
									goto _skip;
								case 'F': case 'f':
									++k;
									path[path_pos + k] = name[k];
									if (!name[k]) {
										printf ("%s: GIF: %s\n",
											__func__, name);
										goto _read_file;
									}
								}
							}
							break;

						case 'J': case 'j':
							++k;
							path[path_pos + k] = name[k];
							switch (name[k]) {
							case '\0':
								goto _skip;
							case 'P': case 'p':
								++k;
								path[path_pos + k] = name[k];
								switch (name[k]) {
								case '\0':
									goto _skip;
								case 'E': case 'e':
									++k;
									path[path_pos + k] = name[k];
									switch (name[k]) {
									case '\0':
										goto _skip;
									case 'G': case 'g':
										goto _maybe_jpeg;
									}
									break;

								case 'G': case 'g':
								_maybe_jpeg:
									++k;
									path[path_pos + k] = name[k];
									if (!name[k]) {
										printf ("%s: JPG: %s\n",
											__func__, name);
										goto _read_file;
									}
								}
							}
							break;

						case 'P': case 'p':
							++k;
							path[path_pos + k] = name[k];
							switch (name[k]) {
							case '\0':
								goto _skip;
							case 'N': case 'n':
								++k;
								path[path_pos + k] = name[k];
								switch (name[k]) {
								case '\0':
									goto _skip;
								case 'G': case 'g':
									++k;
									path[path_pos + k] = name[k];
									if (!name[k]) {
										printf ("%s: PNG: %s\n",
											__func__, name);
										goto _read_file;
									}
								}
							}
						}
					}
				}

				path[path_pos + k] = name[k];

			default:
			_skip:
				continue;
			}

		_read_file:
			if ((im = _cache_add_img_from_disk (cache, path, hash))) {
				char buf[41+37];
				_sha1_str (&hash, buf);

				list_head_t *h = &im->refs;
				list_head_t *h2 = h->next;
				while (h2 != NULL && h2 != h) {
					struct banner *b = list_entry (h2, struct banner, by_hash);
					uuid_unparse_lower (b->uuid, buf + 41);
					if (im->name) {
						if (b->name) {
							printf ("%s: \"%s\" -> \"%s\"\n",
								__func__, im->name, b->name);
						} else {
							printf ("%s: \"%s\" -> %s\n",
								__func__, im->name, buf + 41);
						}
					} else {
						if (b->name) {
							printf ("%s: %s -> \"%s\"\n",
								__func__, buf, b->name);
						} else {
							printf ("%s: %s -> %s\n",
								__func__, buf, buf + 41);
						}
					}
					h2 = h2->next;
				}
				
				printf ("%s: imported: %s\n", __func__, buf);
				break;
			}
		}

	close_dir:
		if (closedir (dirp) == -1) {
			fprintf (stderr, "%s: closedir failed: %s\n",
			         __func__, strerror (errno));
		}

		path[path_pos] = '\0';
		dirp = NULL;

	} else {
		fprintf (stderr, "%s: opendir failed: %s\n", __func__,
		         strerror (errno));
	}
}

__attribute__((always_inline))
static inline bool
_cache_import_subdir (struct cache  *cache,
                      char          *path,
                      size_t         path_pos,
                      struct dirent *entry,
                      uint8_t        id)
{
	bool r;
	DIR *dirp;

	if ((dirp = opendir (path))) {
		for (int i;;) {
			struct dirent *result;
			if ((i = readdir_r (dirp, entry, &result))) {
				fprintf (stderr,
				         "%s: readdir_r failed: %s\n",
				         __func__, strerror (i));
				result = NULL;
				i = 0;
				r = false;
				goto close_dir;
			}

			if (!result) {
				// end of directory stream
				break;
			}

			const char *name;
			size_t k;
			uint8_t c;
			union {
				uint8_t u8[20];
				sha1_t  sha1;
				uuid_t  uuid;
			} v;

			switch (result->d_type) {
			case DT_DIR:
				name = (const char *) result->d_name;
				k = 0;
				v.u8[0] = id;

				do {
					switch (name[k]) {
					case '0' ... '9':
						c = name[k] - '0';
						break;
					case 'A' ... 'F':
						c = 10 + (name[k] - 'A');
						break;
					case 'a' ... 'f':
						c = 10 + (name[k] - 'a');
						break;
					default:
						goto _skip;
					}

					path[path_pos + k] = name[k];
					c <<= 4;
					++k;

					switch (name[k]) {
					case '0' ... '9':
						c |= name[k] - '0';
						break;
					case 'A' ... 'F':
						c |= 10 + (name[k] - 'A');
						break;
					case 'a' ... 'f':
						c |= 10 + (name[k] - 'a');
						break;
					default:
						goto _skip;
					}

					path[path_pos + k] = name[k];
					v.u8[++k >> 1] = c;
				} while (k < 30);

				if (!name[k]) {
					path[path_pos + k++] = '/';
					path[path_pos + k] = '\0';
					_cache_import_banner (cache, path,
					                      path_pos + k,
					                      entry, v.uuid);
					continue;
				}

				do {
					switch (name[k]) {
					case '0' ... '9':
						c = name[k] - '0';
						break;
					case 'A' ... 'F':
						c = 10 + (name[k] - 'A');
						break;
					case 'a' ... 'f':
						c = 10 + (name[k] - 'a');
						break;
					default:
						goto _skip;
					}

					path[path_pos + k] = name[k];
					c <<= 4;
					++k;

					switch (name[k]) {
					case '0' ... '9':
						c |= name[k] - '0';
						break;
					case 'A' ... 'F':
						c |= 10 + (name[k] - 'A');
						break;
					case 'a' ... 'f':
						c |= 10 + (name[k] - 'a');
						break;
					default:
						goto _skip;
					}

					path[path_pos + k] = name[k];
					v.u8[++k >> 1] = c;
				} while (k < 38);

				if (!name[k]) {
					path[path_pos + k++] = '/';
					path[path_pos + k] = '\0';
					_cache_import_img (cache, path,
					                   path_pos + k,
					                   entry, v.sha1);
				}

			default:
			_skip:
				continue;
			}
		}

		r = true;

	close_dir:
		if (closedir (dirp) == -1) {
			fprintf (stderr, "%s: closedir failed: %s\n",
			         __func__, strerror (errno));
		}

		path[path_pos] = '\0';
		dirp = NULL;

	} else {
		fprintf (stderr, "%s: opendir failed: %s\n", __func__,
		         strerror (errno));
		r = false;
	}

	path[path_pos] = '\0';

	return r;
}

__attribute__((always_inline))
static inline bool
_cache_activate_banner (struct cache  *c,
                        struct banner *b)
{
	struct img *im;

	if (!(im = _cache_find_img_by_hash (c, b->hash))) {
		fprintf (stderr, "%s: image hash not in cache\n", __func__);
		return false;
	}

	if (!_banner_activate (b, im)) {
		fprintf (stderr, "%s: failed to activate banner\n", __func__);
		return false;
	}

	printf ("%s: activated banner \"%s\"\n", __func__, b->name);

	return true;
}

__attribute__((always_inline))
static inline bool
_cache_import (struct cache *cache)
{
	const char *root_path = (const char *) cache->path;
	size_t root_len = _cache_path_length (cache), path_alloc;
	char *path;

	if (!(path = _mem_new (6, root_len + 42 + 256, &path_alloc))) {
		return false;
	}

	strncpy (path, root_path, root_len);
	path[root_len] = '\0';

	errno = 0;
	long name_max = pathconf (root_path, _PC_NAME_MAX);
	size_t len;
	uint8_t uuid_str[48];
	bool have_banner_uuid = false;

	if (-1 == name_max) {
		if (errno) {
			fprintf (stderr, "%s: pathconf failed: %s\n", __func__,
			         strerror (errno));
		}
		goto len_to_default;
	} else if (BUFSIZ > offsetof (struct dirent, d_name) + name_max + 1) {
	len_to_default:
		len = BUFSIZ;
	} else {
		len = offsetof (struct dirent, d_name) + name_max + 1;
	}

	name_max = 0;

	bool r;

	struct dirent *entry;
	if ((entry = malloc (len))) {
		DIR *dirp;
		if ((dirp = opendir (root_path))) {
			for (int i;;) {
				struct dirent *result;
				if ((i = readdir_r (dirp, entry, &result))) {
					fprintf (stderr,
					         "%s: readdir_r failed: %s\n",
					         __func__, strerror (i));
					result = NULL;
					i = 0;
					r = false;
					goto close_dir;
				}

				if (!result) {
					// end of directory stream
					break;
				}

				const char *name;
				uint8_t c;

				switch (result->d_type) {
				case DT_DIR:
					name = (const char *) result->d_name;
					switch (name[0]) {
					case '0' ... '9':
						c = name[0] - '0';
						goto _2nd_char;
					case 'A' ... 'F':
						c = 10 + (name[0] - 'A');
						goto _2nd_char;
					case 'a' ... 'f':
						c = 10 + (name[0] - 'a');
					_2nd_char:
						c <<= 4;
						switch (name[1]) {
						case '0' ... '9':
							c |= name[1] - '0';
							goto _3rd_char;
						case 'A' ... 'F':
							c |= 10 + (name[1] - 'A');
							goto _3rd_char;
						case 'a' ... 'f':
							c |= 10 + (name[1] - 'a');
						_3rd_char:
							if (name[2] == '\0') {
								path[root_len] = name[0];
								path[root_len+1] = name[1];
								path[root_len+2] = '/';
								path[root_len+3] = '\0';
								_cache_import_subdir (cache, path, root_len + 3, entry, c);
							}
						default:
							break;
						}
					default:
						break;
					}
					break;

				case DT_REG:
					printf ("%s: DT_REG\n", __func__);
					/* TODO: eventually support several
					 * displays' current banners, but for
					 * now we only ever have display "0"
					 */

					if (result->d_name[0] != '0'
					    || result->d_name[1] != '\0') {
						    break;
					}

					path[root_len] = '0';
					path[root_len+1] = '\0';

					printf ("%s: DT_REG: %s\n", __func__, path);

					if (_textfile_read (path, uuid_str,
					                    48, &len)) {
						have_banner_uuid = true;
					} else {
						fprintf (stderr, "%s: failed to read current banner UUID string from file\n", __func__);
					}

					path[root_len] = '\0';

				case DT_LNK: // TODO: handle symlinks
				default:
					break;
				}
			}

			r = true;

		close_dir:
			if (closedir (dirp) == -1) {
				fprintf (stderr, "%s: closedir failed: %s\n",
				         __func__, strerror (errno));
			}

			dirp = NULL;
		} else {
			fprintf (stderr, "%s: opendir failed: %s\n", __func__,
			         strerror (errno));
			r = false;
		}

		free (entry);
		entry = NULL;
	} else {
		fprintf (stderr, "%s: malloc failed: %s\n",
		         __func__, strerror (errno));
		r = false;
	}

	_mem_del (&path);

	if (have_banner_uuid) {
		struct banner *b;
		if (!(b = _cache_find_banner_by_uuid_str (cache, (const char *) uuid_str))) {
			fprintf (stderr, "%s: failed to find current banner from cache\n", __func__);
		} else if (!_cache_activate_banner (cache, b)) {
			fprintf (stderr, "%s: failed to activate requested banner\n", __func__);
		}
	}

	return r;
}

__attribute__((always_inline))
static inline struct banner *
_cache_most_recent (struct cache *c)
{
	list_head_t *h = &c->used[CACHE_HASH];
	list_head_t *h2 = h->next;
	while (h2 != NULL && h2 != h) {
		struct cache_bucket *cb = list_entry (h2, struct cache_bucket, hook);
		list_head_t *h3 = &cb->list;
		list_head_t *h4 = h3->next;
		while (h3 != NULL && h4 != h3) {
			struct img *im = list_entry (h4, struct img, hook);
			list_head_t *h5 = &im->refs;
			list_head_t *h6 = h5->next;
			if (h5 != NULL && h5 != h6) {
				return list_entry (h6, struct banner, by_hash);
			}
			h4 = h4->next;
		}
		h2 = h2->next;
	}
	return NULL;
}

__attribute__((always_inline))
static inline char *
_cache_json (struct cache *c)
{
	size_t pos, p, len = BUFSIZ;
	union {
		uint8_t *u;
		char    *c;
	} buf;

	if (!(buf.u = malloc (len))) {
		fprintf (stderr, "%s: malloc: %s\n",
		         __func__, strerror (errno));
		return NULL;
	}

	(void) strncpy (buf.c, "{\"b\":[", 6);
	p = pos = 6;

	list_head_t *h = &c->used[CACHE_UUID];
	struct cache_bucket *bkt;
	list_for_each_entry (bkt, h, hook) {
		list_head_t *h2 = &bkt->list;
		struct banner *b;
		list_for_each_entry (b, h2, by_uuid) {
			if (pos > p) {
				buf.c[pos++] = ',';
			}
			(void) _banner_serialize (b, &pos, &len, &buf.u);
		}
	}

	(void) strncpy (buf.c + pos, "],\"i\":[", 7);
	pos += 7;
	p = pos;

	h = &c->used[CACHE_HASH];
	list_for_each_entry (bkt, h, hook) {
		list_head_t *h2 = &bkt->list;
		struct img *im;
		list_for_each_entry (im, h2, hook) {
			if (pos > p) {
				buf.c[pos++] = ',';
			}
			(void) _img_serialize (im, &pos, &len, &buf.u);
		}
	}

	buf.c[pos++] = ']';
	buf.c[pos++] = '}';
	buf.c[pos] = '\0';

	return buf.c;
}

__attribute__((always_inline))
static inline void
_cache_info (struct cache *c,
             size_t       *num_i,
	     size_t       *i_size,
             size_t       *num_b,
	     size_t       *b_size)
{
	list_head_t *h, *h2;
	struct cache_bucket *b;
	union {
		struct img    *i;
		struct banner *b;
	} ptr;
	size_t s;

	h = &c->used[CACHE_HASH];
	*num_i = 0;
	*i_size = 0;

	list_for_each_entry (b, h, hook) {
		h2 = &b->list;
		list_for_each_entry (ptr.i, h2, hook) {
			s = _img_name_length (ptr.i);
			*i_size += (s < UINT32_MAX) ? s : UINT32_MAX;
			(*num_i)++;
		}
	}
	printf ("%s: images: %zu\n", __func__, *num_i);

	*i_size += *num_i * sizeof (struct img_info);

	h = &c->used[CACHE_UUID];
	*num_b = 0;
	*b_size = 0;

	list_for_each_entry (b, h, hook) {
		h2 = &b->list;
		list_for_each_entry (ptr.b, h2, by_uuid) {
			s = _banner_name_length (ptr.b);
			*b_size += (s < UINT32_MAX) ? s : UINT32_MAX;
			(*num_b)++;
		}
	}
	printf ("%s: banners: %zu\n", __func__, *num_b);

	*b_size += *num_b * sizeof (struct banner_info);
}

__attribute__((always_inline))
static inline bool
_cache_info_packet (struct cache  *c,
                    uint8_t      **data,
                    size_t        *size)
{
	size_t num_i, i_size, num_b, b_size, s, n;
	struct cache_packet *p;
	list_head_t *h, *h2;
	struct cache_bucket *b;
	union {
		struct img    *i;
		struct banner *b;
	} ptr;
	uint8_t *buf;

	_cache_info (c, &num_i, &i_size, &num_b, &b_size);
	s = offsetof (struct cache_packet, data) + i_size + b_size;

	if (!(p = _mem_new (6, s, &n))) {
		return false;
	}

	p->time = 0;
	p->inum = num_i;
	p->bnum = num_b;

	if (cur_banner) {
		_banner_uuid_copy_to_u32 (cur_banner, p->uuid);
	} else {
		p->uuid[0] = 0;
		p->uuid[1] = 0;
		p->uuid[2] = 0;
		p->uuid[3] = 0;
	}

	*size = s;
	*data = (uint8_t *) p;

	h = &c->used[CACHE_HASH];
	buf = *data + offsetof (struct cache_packet, data);

	list_for_each_entry (b, h, hook) {
		h2 = &b->list;
		list_for_each_entry (ptr.i, h2, hook) {
			struct img_info *ii = (struct img_info *) buf;
			_img_info_init (ii, ptr.i);
			buf += sizeof (struct img_info);
		}
	}

	h = &c->used[CACHE_UUID];

	list_for_each_entry (b, h, hook) {
		h2 = &b->list;
		list_for_each_entry (ptr.b, h2, by_uuid) {
			struct banner_info *bi = (struct banner_info *) buf;
			_banner_info_init (bi, ptr.b);
			buf += sizeof (struct banner_info);
		}
	}

	h = &c->used[CACHE_HASH];

	list_for_each_entry (b, h, hook) {
		h2 = &b->list;
		list_for_each_entry (ptr.i, h2, hook) {
			s = _img_name_length (ptr.i);
			if (s >= UINT32_MAX) {
				s = UINT32_MAX;
			}
			memcpy (buf, ptr.i->name, s);
			buf += s;
		}
	}

	h = &c->used[CACHE_UUID];

	list_for_each_entry (b, h, hook) {
		h2 = &b->list;
		list_for_each_entry (ptr.b, h2, by_uuid) {
			s = _banner_name_length (ptr.b);
			if (s >= UINT32_MAX) {
				s = UINT32_MAX;
			}
			memcpy (buf, ptr.b->name, s);
			buf += s;
		}
	}

	return true;
}

__attribute__((always_inline))
static inline bool
_cache_import_packet (struct cache         *c,
                      struct banner_packet *p)
{
	char str[48];
	struct img *im;
	struct banner *b;

	_sha1_str (&p->hash, str);
	printf ("time: %lu\nxpos: %d\nypos: %d\nwidth: %d\n"
	        "height: %d\nhash: %s\nsize: %lu\nname len: %u\nfilename len: %u\n",
	        p->time, p->offs.x, p->offs.y,
	        _geo2d_width (&p->dims), _geo2d_height (&p->dims), str, p->size,
	        p->nsiz, p->fsiz);

	if (!(im = _cache_find_or_add_img_from_packet (c, p))) {
		fprintf (stderr, "%s: failed to extract image from packet\n",
		         __func__);
		return false;
	}

	if (!(b = _banner_create_from_packet (p))) {
		fprintf (stderr, "%s: failed to create banner object\n",
		         __func__);
		return false;
	}

	if (!_cache_add_banner (c, b, im)) {
		fprintf (stderr, "%s: failed to add banner to cache\n",
		         __func__);
		_banner_destroy (b);
		return false;
	}

	if (!_cache_export_banner (c, b)) {
		fprintf (stderr, "%s: failed to export banner to disk\n",
		         __func__);
		_banner_destroy (b);
		return false;
	}

	if (!_banner_activate (b, im)) {
		fprintf (stderr, "%s: failed to activate banner\n", __func__);
		return false;
	}

	const char *root_path = (const char *) c->path;
	size_t root_len = _cache_path_length (c), alloc_size;
	char *path;

	if (!(path = _mem_new (4, root_len + 2, &alloc_size))) {
		fprintf (stderr, "%s: failed to allocate memory\n",
		         __func__);
	} else {
		strncpy (path, root_path, root_len);
		(void) memset (path + root_len, 0, alloc_size - root_len);

		if (!display_banner_uuid_export (path, root_len)) {
			fprintf (stderr,
			         "%s: failed to export current banner UUID\n",
			         __func__);
		}

		free (path);
	}

	return true;
}

__attribute__((always_inline))
static inline bool
_cache_apply_modpkt (struct cache          *c,
                     struct banner_modpkt  *p,
                     char                 **r,
                     size_t                *s)
{
	struct banner *b;
	struct img *im;
	union {
		struct banner *b;
		uint8_t       *u8;
		const char    *cc;
		list_head_t   *h;
	} ptr;
	struct {
		sha1_t       *hash;
		struct geo2d *dims;
		point_t      *offs;
		char         *name;
		size_t        name_len;
	} modded;
	char *path;
	size_t path_len, reply_size, alloc_size;
	struct banner_modpkt_echo *reply;

	if (!(b = _cache_find_banner_by_uuid (c, p->uuid))) {
		fprintf (stderr, "%s: packet refers to unknown banner\n",
		         __func__);
		return false;
	}

	if (!_sha1_cmp (&p->hash, &b->hash)) {
		printf ("%s: banner image change request received\n", __func__);

		if (!(im = _cache_find_img_by_hash (c, p->hash))) {
			fprintf (stderr, "%s: packet refers to unknown image\n",
			         __func__);
			return false;
		}

		list_for_each_entry (ptr.b, &im->refs, by_hash) {
			if (ptr.b == b || _sha1_cmp (&ptr.b->hash, &b->hash)) {
				fprintf (stderr, "%s: image already references "
				         "banner with the same UUID\n",
				         __func__);
				return false;
			}
		}

		modded.hash = &p->hash;

	} else {
		// packet does not contain a request to change the banner image
		modded.hash = NULL;
	}

	if (p->nsiz > 0) {
		ptr.u8 = p->name;
		if (!_banner_name_dup (ptr.cc, p->nsiz,
		                       &modded.name, &modded.name_len)) {
			fprintf (stderr, "%s: failed to copy banner name\n",
			         __func__);
			return false;
		}

		if (b->name && b->name_len == modded.name_len
		    && strncmp (modded.name, b->name, modded.name_len) == 0) {
			free (modded.name);
			goto _name_not_changed;
		}

	} else {
	_name_not_changed:
		modded.name = NULL;
		modded.name_len = 0;
	}

	reply_size = offsetof (struct banner_modpkt_echo, orig.name)
	             + modded.name_len;

	if (!(reply = _mem_new (6, reply_size, &alloc_size))) {
		if (modded.name) {
			free (modded.name);
		}
		fprintf (stderr, "%s: failed to allocate reply packet\n",
		         __func__);
		return false;
	}

	if (modded.hash) {
		ptr.h = &b->by_hash;

		if (ptr.h->prev != ptr.h
		    && ptr.h->next != ptr.h
		    && ptr.h->prev && ptr.h->next) {
			__list_del_entry (ptr.h);
		}

		_sha1_cpy (modded.hash, &b->hash);
		list_add (ptr.h, &im->refs);
	} else {
		im = _cache_find_img_by_hash (c, b->hash);
	}

	if (modded.name) {
		if (b->name) {
			free (b->name);
		}

		b->name = modded.name;
		b->name_len = modded.name_len;
	}

	if (p->offs.u64 != b->offset.u64) {
		b->offset.u64 = p->offs.u64;
		modded.offs = &b->offset;
	} else {
		modded.offs = NULL;
	}

	if (p->dims.u64 != b->dims.u64 && p->dims.w > 0 && p->dims.h > 0) {
		b->dims.u64 = p->dims.u64;
		modded.dims = &b->dims;
	} else {
		modded.dims = NULL;
	}

	if (modded.hash || modded.dims || modded.offs || modded.name) {
		printf ("%s: modified:", __func__);
		if (modded.hash) {
			fputs (" hash", stdout);
		}
		if (modded.dims) {
			fputs (" dims", stdout);
		}
		if (modded.offs) {
			fputs (" offs", stdout);
		}
		if (modded.name) {
			fputs (" name", stdout);
		}
		puts("");

		if (_cache_banner_path (c, b, &path, &path_len)) {
			_banner_export_modifications (modded.hash, modded.dims,
			                              modded.offs, modded.name,
			                              modded.name_len, path,
			                              path_len);
			free (path);
			path = NULL;
			path_len = 0;
		}
		if (cur_banner == b && im) {
			if (!_banner_activate (b, im)) {
				fprintf (stderr,
				         "%s: failed to activate banner\n",
				         __func__);
			}
		}
	}

	reply->type = PACKET_MOD|PACKET_BANNER;
	reply->orig.time = p->time;
	uuid_copy (reply->orig.uuid, b->uuid);
	reply->orig.offs.u64 = b->offset.u64;
	reply->orig.dims.u64 = b->dims.u64;
	_sha1_cpy (&b->hash, &reply->orig.hash);
	reply->orig.nsiz = modded.name_len;
	if (modded.name_len > 0 && modded.name) {
		(void) memcpy (reply->orig.name, modded.name, modded.name_len);
	}
	if (reply_size < alloc_size) {
		(void) memset (((char *) reply) + reply_size, 0,
		               alloc_size - reply_size);
	}

	*r = (char *) reply;
	*s = reply_size;

	return true;
}

__attribute__((always_inline))
static inline bool
_cache_apply_display_modpkt (struct cache           *c,
                             struct display_modpkt  *p,
                             char                  **r,
                             size_t                 *s)
{
	struct banner *b;
	const char *root_path;
	size_t root_len, path_len, reply_size, alloc_size;
	char *path;
	struct display_modpkt_echo *reply;

	if (!(b = _cache_find_banner_by_uuid (c, p->uuid))) {
		fprintf (stderr, "%s: packet refers to unknown banner\n",
		         __func__);
		return false;
	}

	printf ("%s: target banner is \"%s\"\n", __func__, b->name);

	if (cur_banner == b) {
		fprintf (stderr, "%s: requested banner is already active\n",
		         __func__);
		return false;
	}

	if (!_cache_activate_banner (c, b)) {
		fprintf (stderr, "%s: failed to activate requested banner\n",
		         __func__);
		return false;
	}

	root_path = (const char *) c->path;
	root_len = _cache_path_length (c);
	path_len = root_len + 2;
	reply_size = offsetof (struct display_modpkt_echo, orig.name);

	if (!(path = _mem_new (4,
	                       (path_len > reply_size) ? path_len : reply_size,
	                       &alloc_size))) {
		fprintf (stderr, "%s: failed to allocate memory\n",
		         __func__);
		return false;
	}

	strncpy (path, root_path, root_len);
	(void) memset (path + root_len, 0, alloc_size - root_len);

	if (!display_banner_uuid_export (path, root_len)) {
		fprintf (stderr, "%s: failed to export current banner UUID\n",
		         __func__);
	}

	(void) memset (path, 0, root_len);

	reply = (struct display_modpkt_echo *) path;
	reply->type = PACKET_MOD|PACKET_DISPLAY;
	reply->orig.time = p->time;
	reply->orig.id = 0;
	uuid_copy (reply->orig.uuid, b->uuid);
	reply->orig.dims.u64 = 0; // TODO
	reply->orig.offs.u64 = 0;
	reply->orig.nsiz = 0; // TODO

	*r = (char *) reply;
	*s = reply_size;

	return true;
}

#endif // __KIVIJALKA_PRIVATE_CACHE_H__
