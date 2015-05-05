/** \file watcher.c
 *
 * File change watcher.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <sys/inotify.h>
#include <sys/time.h>

#include "watcher.h"
#include "path.h"

struct watcher {
	int              wd;
	sigset_t         ss;
	struct pollfd    pf;
	struct timespec  to;
	path_head_t      path;
	path_node_t     *trig;
	const char      *file;
	size_t           end;
	char             data[];
} __attribute__((gcc_struct,packed));

__attribute__((always_inline,pure))
static inline size_t
watcher_size (const size_t len)
{
	return sizeof (int)
	       + sizeof (sigset_t)
	       + sizeof (struct pollfd)
	       + sizeof (struct timespec)
	       + sizeof (path_head_t)
	       + sizeof (path_node_t *)
	       + sizeof (const char *)
	       + sizeof (size_t)
	       + len + 1;
}

watcher_t *
watcher_create (const char *path)
{
	watcher_t *w;
	int fd, wd;
	size_t len;
	path_head_t head;
	path_node_t *trig;
	const char *file;

	if (!path) {
		fprintf (stderr, "%s: null path\n", __func__);
		return NULL;
	}

	if (path[0] != '/') {
		fprintf (stderr, "%s: path must be absolute\n", __func__);
		return NULL;
	}

	path_create (&head, path);

	if (path_empty (&head)) {
		fprintf (stderr, "%s: path_create failed\n", __func__);
		return NULL;
	}

	len = path_strlen (&head);

	if (!(w = malloc (watcher_size (len)))) {
		fprintf (stderr, "%s: malloc failed: %s\n",
		                 __func__, strerror (errno));
		path_destroy (&head);
		return NULL;
	}

	fd = inotify_init1 (IN_NONBLOCK);

	switch (fd) {
	case -1:
		fprintf (stderr, "%s: inotify_init: %s\n",
		                 __func__, strerror (errno));
		goto fail_err;

	default:
		trig = path_node (head.list.prev);
		path_strcpy (w->data, &head);

	try_add_watch:
		file = path_node_name (trig);

		if (path_node_is_first (&head, trig)) {
			++file;
			len = 1;
		} else {
			len -= path_node_strlen (trig) + 1;
		}

		w->data[len] = '\0';

		printf ("Trying to watch %s for %s\n", w->data, file);

		wd = inotify_add_watch (fd, w->data, IN_ALL_EVENTS);
/*
		if (path_node_is_last (&head, trig)) {
			printf ("IN_CLOSE_WRITE|IN_MOVED_TO)\n");
			wd = inotify_add_watch (fd, w->data,
			                        IN_CLOSE_WRITE|IN_MOVED_TO);
		} else {
			printf ("IN_CREATE|IN_MOVED_TO)\n");
			wd = inotify_add_watch (fd, w->data,
			                        IN_CREATE|IN_MOVED_TO);
		}
*/
		switch (wd) {
		case -1:
			fprintf (stderr, "%s: inotify_add_watch: %s\n",
			                 __func__, strerror (errno));

			if (ENOENT == errno
			    && !path_node_is_first (&head, trig)) {
				trig = path_node (trig->list.prev);
				goto try_add_watch;
			}

		fail_err:
			path_destroy (&head);
			free (w);
			w = NULL;
			goto end;

		default:
			if (0 != sigfillset (&w->ss)) {
				fprintf (stderr, "%s: sigfillset: %s\n",
				         __func__, strerror (errno));
				(void) inotify_rm_watch (fd, wd);
				goto fail_err;
			}

			w->wd = wd;
			w->pf.fd = fd;
			w->pf.events = POLLIN;
			w->to.tv_sec = 0;
			w->to.tv_nsec = 500000000;
			path_move (&(w->path), &head);
			w->trig = trig;
			trig = NULL;
			w->file = file;
			file = NULL;
			w->end = len;
		}
	}

end:
	return w;
}

__attribute__((always_inline))
static inline void
watcher_handle_parent_dir (watcher_t *w,
                           char      *b,
                           ssize_t    l)
{
	printf ("%s\n", __func__);

	const struct inotify_event *e;
	uint32_t m = 0;

	for (char *p = b; p < b + l;
	     p += sizeof(struct inotify_event) + e->len) {
		e = (const struct inotify_event *) p;
		if ((e->mask & IN_ISDIR)) {
			if ((e->mask & (IN_CREATE|IN_MOVED_TO))
			    && e->len && !strcmp (e->name, w->file)) {
				m |= (e->mask & (IN_CREATE|IN_MOVED_TO));
			}
/*
		} else {
			// TODO: handle symlink creations, too
*/
		}
	}

	if (m) {
		size_t n = path_node_strlen (w->trig);

		printf ("%s: OLD: w->data=%s | w->end=%zu | w->file=%s | w->trig=%s\n",
		        __func__, w->data, w->end, w->file, path_node_name (w->trig));

		if (path_node_is_first (&(w->path), w->trig)) {
			w->data[1] = path_node_name (w->trig) [1];
			w->end = n;
		} else {
			w->data[w->end] = '/';
			w->end += (n + 1);
		}

		w->trig = path_node_next (w->trig);
		w->file = path_node_name (w->trig);

		printf ("%s: NEW: w->data=%s | w->end=%zu | w->file=%s | w->trig=%s\n",
		        __func__, w->data, w->end, w->file, path_node_name (w->trig));

		// TODO: remove old watch, add new
	}
}

__attribute__((always_inline))
static inline int64_t
watcher_handle_events (watcher_t *w)
{
	char buf[1024]
	__attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	ssize_t len;
	char *ptr;
	int64_t em;

	/* Loop while events can be read from inotify file descriptor. */

	for (;;) {
		if ((len = read (w->pf.fd, buf, sizeof buf)) == -1) {
			printf ("read %ld bytes\n", len);
			if (errno == EAGAIN) {
				printf ("errno == EAGAIN\n");
				continue;
			} else {
				fprintf (stderr, "%s: read: %s\n",
				         __func__, strerror (errno));
				return -1;
			}
		}

		if (len <= 0) {
			break;
		}

		printf ("%s: w->trig=%s\n", __func__, path_node_name (w->trig));

		if (!path_node_is_last (&(w->path), w->trig)) {
			watcher_handle_parent_dir (w, buf, len);
			break;
		}

		/* Loop over all events in the buffer */

		for (em = 0, ptr = buf; ptr < buf + len;
		     ptr += sizeof(struct inotify_event) + event->len) {
			event = (const struct inotify_event *) ptr;
			if (event->mask) {
				if ((event->mask & IN_ACCESS))
					printf ("IN_ACCESS ");
				if ((event->mask & IN_MODIFY))
					printf ("IN_MODIFY ");
				if ((event->mask & IN_ATTRIB))
					printf ("IN_ATTRIB ");
				if ((event->mask & IN_CLOSE_WRITE))
					printf ("IN_CLOSE_WRITE ");
				if ((event->mask & IN_CLOSE_NOWRITE))
					printf ("IN_CLOSE_NOWRITE ");
				if ((event->mask & IN_OPEN))
					printf ("IN_OPEN ");
				if ((event->mask & IN_MOVED_FROM))
					printf ("IN_MOVED_FROM ");
				if ((event->mask & IN_MOVED_TO))
					printf ("IN_MOVED_TO ");
				if ((event->mask & IN_CREATE))
					printf ("IN_CREATE ");
				if ((event->mask & IN_DELETE))
					printf ("IN_DELETE ");
				if ((event->mask & IN_DELETE_SELF))
					printf ("IN_DELETE_SELF ");
				if ((event->mask & IN_MOVE_SELF))
					printf ("IN_MOVE_SELF ");
				if ((event->mask & IN_UNMOUNT))
					printf ("IN_UNMOUNT ");
				if ((event->mask & IN_Q_OVERFLOW))
					printf ("IN_Q_OVERFLOW ");
				if ((event->mask & IN_IGNORED))
					printf ("IN_IGNORED ");

				if ((event->mask & IN_ISDIR)) {
					printf ("(dir) ");
				} else if ((event->mask & (IN_CLOSE_WRITE|IN_MOVED_TO))
				           && event->len
				           && !strcmp (event->name, w->file)) {
						em |= event->mask;
				}

				if (event->len && event->name) {
					printf ("%s", event->name);
					if (!strcmp (event->name, w->file)) {
						printf (" *\n");
					} else {
						puts("");
					}
				} else {
					puts("");
				}
			}
		}

		return em;
	}

	return 0;
}

int64_t
watcher_run_once (watcher_t *w)
{
	int r = ppoll (&w->pf, (nfds_t)1, &w->to, &w->ss);
	switch (r) {
	case -1:
		if (errno != EINTR) {
			fprintf (stderr, "%s: ppoll: %s\n",
			         __func__, strerror (errno));
			return -1;
		}
	case 0:
		return 0;
	default:
		return ((w->pf.revents & POLLIN))
		       ? watcher_handle_events (w) : 0;
	}
}

void
watcher_destroy (watcher_t *w)
{
	if (w) {
		w->file = NULL;
		(void) memset ((void *) w->data, 0,
		               path_strlen (&(w->path)));
		path_destroy (&(w->path));
		if (0 != inotify_rm_watch (w->pf.fd, w->wd)) {
			fprintf (stderr, "%s: inotify_rm_watch: %s\n",
			         __func__, strerror (errno));
		}
		close (w->pf.fd);
		free (w);
	}
}
