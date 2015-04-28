#include "path.h"

void
path_create (path_head_t *head,
             const char  *path)
{
	char *p = (char *) path, *s;

	switch (*p) {
	case '\0':
		return;

	case '/':
		for (; '/' == *++p;) {
		}
		s = p-1;
		break;

	default:
		s = p++;
	}

	for (;;) {
		switch (*p) {
		case '\0':
			path_add_node (head, path_substr_len (s, p), s);
			return;

		case '/':
			path_add_node (head, path_substr_len (s, p), s);
			for (; '/' == *++p;) {}
			s = p;
			continue;

		default:
			++p;
		}
	}
}

void
path_destroy (path_head_t *head)
{
	path_node_t *n, *n2;
	list_for_each_entry_safe_reverse (n, n2, &(head->list), list) {
		path_del_node (head, n);
	}
}
