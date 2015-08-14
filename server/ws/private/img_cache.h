#ifndef __KIVIJALKA_PRIVATE_IMG_CACHE_H__
#define __KIVIJALKA_PRIVATE_IMG_CACHE_H__

#ifdef __cplusplus
#error This header must not be included directly by C++ code
#endif

#include "../list.h"
#include "cache.h"
#include "sha1.h"
#include "mem.h"
#include "banner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <inttypes.h>

#ifndef _DIRENT_HAVE_D_TYPE
#error The dirent structure does not have a d_type field on this platform
#endif

struct img_cache {
	struct cache_table by_hash;
};

struct img_entry {
	list_head_t  hook;   //! List hook
	sha1_t       hash;   //! Hash of this image
	char        *name;   //! Name of this image's source file
	list_head_t  refs;   //! List of all banners using this image
	size_t       size;   //! Size of image file in bytes
	uint8_t     *data;   //! Pointer to memory buffer containing image file
	size_t       width;  //! Width of this image in pixels
	size_t       height; //! Height of this image in pixels
	uint8_t     *pxdata; //! Pointer to memory buffer containing pixel data
} __attribute__((gcc_struct,packed));

__attribute__((always_inline))
static inline void
_img_entry_init (struct img_entry *e,
                 list_head_t      *list,
                 sha1_t           *hash,
                 char             *name,
                 size_t            size,
                 uint8_t          *data,
                 size_t            width,
                 size_t            height,
                 uint8_t          *pxdata)
{
	e->pxdata = pxdata;
	e->height = height;
	e->width = width;

	e->data = data;
	e->size = size;

	list_init (&e->refs);

	e->name = name;

	if (hash) {
		_sha1_cpy (hash, &e->hash);
	} else {
		_sha1_init (&e->hash);
	}

	if (list) {
		list_add (&e->hook, list);
	} else {
		list_init (&e->hook);
	}
}

__attribute__((always_inline))
static inline char *
_img_name_dup (const char *name)
{
	size_t i, j;
	char c, *n;
	bool mangle;

	for (i = 0; (c = name[i]); ++i) {
		if (c != '/') {
			continue;
		}
		for (; name[++i];);
		mangle = true;
		goto _alloc_name;
	}

	mangle = false;

_alloc_name:
	if (!(n = _mem_new (3, i + 1, &j))) {
		fprintf (stderr, "%s: string allocation failed\n", __func__);
		return NULL;
	}

	printf ("%s: allocated %zu B\n", __func__, j);

	if (!mangle) {
		(void) strncpy (n, name, i);
	} else {
		for (j = 0; j < i; ++j) {
			c = name[j];
			n[j] = (c != '/') ? c : '_';
		}
	}
	n[i] = '\0';

	return n;
}

__attribute__((always_inline))
static inline struct img_entry *
_img_entry_create (list_head_t *list,
                   sha1_t      *hash,
                   const char  *name,
                   size_t       size,
                   uint8_t     *data,
                   size_t       width,
                   size_t       height,
                   uint8_t     *pxdata)
{
	struct img_entry *e;
	size_t n;
	char *_name;

	if (!(e = _mem_new (6, sizeof (struct img_entry), &n))) {
		fprintf (stderr, "%s: object allocation failed\n", __func__);
		return NULL;
	}

	printf ("%s: allocated %zu B\n", __func__, n);

	if ((_name = _img_name_dup (name))) {
		_img_entry_init (e, list, hash, _name, size,
		                 data, width, height, pxdata);
		_name = NULL;
	} else {
		fprintf (stderr, "%s: _img_name_dup failed\n", __func__);
		_mem_del (&e);
	}

	return e;
}

__attribute__((always_inline))
static inline void
_img_entry_destroy (struct img_entry *e)
{
	if (e->hook.next && e->hook.prev) {
		list_del (&e->hook);
	}

	struct banner *b, *tmp;
	list_for_each_entry_safe (b, tmp, &e->refs, by_hash) {
		list_del (&b->by_hash);
		_sha1_init (&b->hash);
	}
	e->refs.next = NULL;
	e->refs.prev = NULL;

	if (e->name) {
		free (e->name);
		e->name = NULL;
	}

	_sha1_init (&e->hash);

	e->size = 0;
	if (e->data) {
		free (e->data);
		e->data = NULL;
	}

	e->width = 0;
	e->height = 0;
	if (e->pxdata) {
		free (e->pxdata);
		e->pxdata = NULL;
	}

	free (e);
}

	__attribute__((always_inline))
static inline struct img_cache *
_img_cache_create (void)
{
	struct img_cache *ic;
	size_t n;
	struct cache_bucket *lt;

	if (!(ic = _mem_new (6, sizeof (struct img_cache), &n))) {
		fprintf (stderr, "%s: object allocation failed\n", __func__);
		return NULL;
	}

	printf ("%s: allocated %zu B\n", __func__, n);

	if (!(lt = _mem_new (6, 256 * sizeof (struct cache_bucket), &n))) {
		fprintf (stderr, "%s: lookup table allocation failed\n",
		         __func__);
		_mem_del (&ic);
		return NULL;
	}

	printf ("%s: allocated %zu B\n", __func__, n);

	for (unsigned int i = 0; i < 256; ++i) {
		list_init (&lt[i].hook);
		list_init (&lt[i].list);
	}

	list_init (&ic->by_hash.in_use);
	ic->by_hash.lookup_table = lt;
	lt = NULL;

	return ic;
}

__attribute__((always_inline))
static inline void
_img_cache_destroy (struct img_cache *ic)
{
	struct cache_table *ct = &ic->by_hash;
	struct cache_bucket *bkt, *tmp;
	struct img_entry *e, *tmp2;

	list_for_each_entry_safe (bkt, tmp, &ct->in_use, hook) {
		list_for_each_entry_safe (e, tmp2, &bkt->list, hook) {
			_img_entry_destroy (e);
		}
		__list_del_entry (&bkt->hook);
	}
	ic->by_hash.in_use.next = NULL;
	ic->by_hash.in_use.prev = NULL;

	bkt = NULL;
	tmp = NULL;
	e = NULL;
	tmp2 = NULL;

	if (ct->lookup_table) {
		(void) memset (ct->lookup_table, 0,
		               256 * sizeof (struct cache_bucket));
		free (ct->lookup_table);
		ct->lookup_table = NULL;
	}

	free (ic);
	ic = NULL;
}

__attribute__((always_inline))
static inline struct cache_bucket *
_img_cache_bucket (struct img_cache *ic,
                   sha1_t            hash)
{
	return &ic->by_hash.lookup_table[hash.u8[0]];
}

__attribute__((always_inline))
static inline struct img_entry *
_entry_by_hash (list_head_t *list,
                sha1_t       hash)
{
	struct img_entry *e;
	list_for_each_entry (e, list, hook) {
		if (_sha1_cmp (&e->hash, &hash)) {
			return e;
		}
	}
	return NULL;
}

__attribute__((always_inline))
static inline struct img_entry *
_img_cache_find (struct img_cache *ic,
                 sha1_t            hash)
{
	struct cache_bucket *bkt = _img_cache_bucket (ic, hash);
	return _entry_by_hash (&bkt->list, hash);
}

__attribute__((always_inline))
static inline bool
_img_entry_export (struct img_entry *entry,
                   const char       *root_path,
                   size_t            path_len)
{
	bool r;
	union {
		char       *c;
		const char *cc;
	} name;
	size_t name_len, path_alloc;
	char *path;
	const uint8_t *id;

	if (entry->name) {
		name.c = entry->name;
		if ((name_len = strlen (name.cc)) < 1) {
			goto _no_entry_name;
		}
	} else {
	_no_entry_name:
		name.cc = "i";
		name_len = 1;
	}

	if (!(path = _mem_new (6, path_len + 43 + name_len, &path_alloc))) {
		fprintf (stderr, "%s: can't export image: out of memory\n",
		         __func__);
		return false;
	}

	printf ("%s: allocated %zu B\n", __func__, path_alloc);

	strncpy (path, root_path, path_len);
	path[path_len] = '\0';

	id = ((const uint8_t *) entry) + offsetof (struct img_entry, hash);

	if (!_cache_mkdir (path, path_len, 20, id)) {
		fprintf (stderr, "%s: failed to create image cache subdir\n",
		         __func__);
		r = false;
		goto _destroy_path;
	}

	path[path_len + 41] = '/';
	strncpy (path + path_len + 42, name, name_len);
	path[path_len + 42 + name_len] = '\0';

	printf ("%s: writing %s\n", __func__, path);

_destroy_path:
	_mem_del (&path);

	return r;
}

__attribute__((always_inline))
static inline bool
_img_cache_add_entry (struct img_cache *ic,
                      struct img_entry *entry,
                      const char       *root_path,
                      size_t            path_len)
{
	struct cache_bucket *bkt = _img_cache_bucket (ic, entry->hash);
	struct img_entry *e;

	if ((e = _entry_by_hash (&bkt->list, entry->hash))) {
		char str[41], str2[41];
		_sha1_str (&entry->hash, str);
		_sha1_str (&e->hash, str2);
		fprintf (stderr,
		         "%s: hash collision:\n"
		         ">       %p SHA1=%s\n"
		         ">          name=%s\n"
		         ">   vs. %p SHA1=%s\n"
		         ">          name=%s\n",
		         __func__, entry, str, entry->name, e, str2, e->name);
		return false;
	}

	list_add (&entry->hook, &bkt->list);

	if (bkt->hook.next == &bkt->hook) {
		list_add (&bkt->hook, &ic->by_hash.in_use);
	}

	/* if root_path is non-NULL the caller
	 * wants us to write the image to disk
	 */
	if (root_path) {
		if (!_img_entry_export (entry, root_path, path_len) {
			fprintf (stderr, "%s: image export failed\n", __func__);
			return false;
		}
	}

	return true;
}

#endif // __KIVIJALKA_PRIVATE_IMG_CACHE_H__
