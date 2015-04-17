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

struct watcher {
	int             wd;
	sigset_t        ss;
	struct pollfd   pf;
	struct timespec to;
	char            file[];
} __attribute__((gcc_struct,packed));

watcher_t *
watcher_create (const char *path)
{
	watcher_t *w;
	int fd, wd;
	size_t dir_len, file_len;
	char *p;
	const char *file;

	if (!path || !path[0]) {
		fprintf (stderr, "%s: invalid arguments\n", __func__);
		return NULL;
	}

	p = (char *) path;
	file = (const char *) p - 1;
	for (; *p; ++p) {
		if ('/' == *p) {
			file = (const char *) p;
		}
	}

	++file;
	file_len = (size_t) ((ptrdiff_t) p - (ptrdiff_t) file);
	dir_len = (size_t) ((ptrdiff_t) file - (ptrdiff_t) path);

	if (dir_len) {
		if (!(p = malloc (dir_len + 1))) {
			return NULL;
		}
		strncpy (p, path, dir_len);
		p[dir_len] = '\0';
	} else {
		if (!(p = malloc (3))) {
			return NULL;
		}
		strncpy (p, "./", 2);
		p[2] = '\0';
	}

	if (!(w = malloc (2 * sizeof (int) + file_len + 1))) {
		goto end;
	}

	strncpy (w->file, file, file_len);
	w->file[file_len] = '\0';

//	printf ("path: %s\nfile: %s\n", p, w->file);

	fd = inotify_init1 (IN_NONBLOCK);

	switch (fd) {
	case -1:
		goto fail_err;

	default:
		wd = inotify_add_watch (fd, p, IN_CLOSE_WRITE|IN_MOVED_TO|IN_MASK_ADD);

		switch (wd) {
		case -1:
		fail_err:
			fprintf (stderr, "%s: %s\n", __func__, strerror (errno));
			free (w);
			w = NULL;
			goto end;

		default:
			if (0 != sigfillset (&w->ss)) {
				fprintf (stderr, "%s: sigfillset: %s\n",
				         __func__, strerror (errno));
				goto fail_err;
			}

			w->wd = wd;
			w->pf.fd = fd;
			w->pf.events = POLLIN;
			w->to.tv_sec = 0;
			w->to.tv_nsec = 500000000;
		}
	}

end:
	free (p);
	return w;
}

__attribute__((always_inline))
static inline uint32_t
watcher_handle_events (watcher_t *w)
{
	char buf[1024]
	__attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	ssize_t len;
	char *ptr;
	uint32_t em;

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
				break;
			}
		}

		if (len <= 0) {
			break;
		}

		/* Loop over all events in the buffer */

		for (em = 0, ptr = buf; ptr < buf + len;
		     ptr += sizeof(struct inotify_event) + event->len) {
			event = (const struct inotify_event *) ptr;
			if (event->mask && !(event->mask & IN_ISDIR)
			    && event->len && !strcmp (event->name, w->file)) {
				em |= event->mask;
			}
		}

		return em;
	}

	return 0;
}

uint32_t
watcher_run_once (watcher_t *w)
{
	int r = ppoll (&w->pf, (nfds_t)1, &w->to, &w->ss);
	switch (r) {
	case -1:
		if (errno != EINTR) {
			fprintf (stderr, "%s: ppoll: %s\n",
			         __func__, strerror (errno));
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
	close (w->pf.fd);
	free (w);
}
