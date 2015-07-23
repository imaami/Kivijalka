/** \file banner.c
 *
 * Banner object.
 */

#include "private/banner.h"

struct banner *
banner_create (void)
{
	return _banner_create ();
}

struct banner *
banner_create_from_path (const char *path)
{
	return (path) ? _banner_create_from_path (path) : NULL;
}

struct banner *
banner_create_from_packet (struct banner_packet *pkt)
{
	return (pkt) ? _banner_create_from_packet (pkt) : NULL;
}

void
banner_destroy (struct banner **b)
{
	if (b && *b) {
		_banner_destroy (*b);
		*b = NULL;
	}
}

bool
banner_uuid_cpy (struct banner *b,
                 uuid_t         dest)
{
	if (b) {
		_banner_uuid_cpy (b, dest);
		return true;
	}
	return false;
}

bool
banner_set_name (struct banner *b,
                 const char    *name)
{
	if (b && name) {
		return _banner_set_name (b, name);
	}
	return false;
}

const char *
banner_name (struct banner *b)
{
	return (b) ? (const char *) b->name : NULL;
}

void
banner_set_offset (struct banner *b,
                   point_t        offset)
{
	if (b) {
		_banner_set_offset (b, offset);
	}
}

bool
banner_hash_cmp (struct banner *b,
                 sha1_t        *hash)
{
	return (b && hash) ? _banner_hash_cmp (b, hash) : false;
}

bool
banner_hash_cpy (struct banner *b,
                 sha1_t        *dest)
{
	if (b && dest) {
		_banner_hash_cpy (b, dest);
		return true;
	}
	return false;
}

void
banner_uuid_unparse (struct banner *b,
                     char          *str)
{
	if (b && str) {
		_banner_uuid_unparse (b, str);
	}
}

void
banner_hash_unparse (struct banner *b,
                     char          *str)
{
	if (b && str) {
		_banner_hash_unparse (b, str);
	}
}

img_data_t *
banner_remove_data (struct banner *b)
{
	return (b) ? _banner_remove_data (b) : NULL;
}
