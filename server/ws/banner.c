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

void
banner_destroy (struct banner **b)
{
	if (b && *b) {
		_banner_destroy (*b);
		*b = NULL;
	}
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

void
banner_print_hash (struct banner *b)
{
	if (b) {
		_banner_print_hash (b);
	}
}

struct banner *
banner_next_in_list (struct banner *b,
                     list_head_t   *list)
{
	if (list) {
		if (b) {
			if (b->hook.next && b->hook.next != list) {
				return list_entry (b->hook.next, struct banner, hook);
			}
		} else if (list->next && list->next != list) {
			return list_first_entry (list, struct banner, hook);
		}
	}
	return NULL;
}

void
banner_add_to_list (struct banner *b,
                    list_head_t   *list)
{
	if (b && list) {
		_banner_add_to_list (b, list);
	}
}

void
banner_del_from_list (struct banner *b)
{
	if (b) {
		_banner_del_from_list (b);
	}
}
