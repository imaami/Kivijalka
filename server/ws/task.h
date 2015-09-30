/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
/** \file task.h
 *
 * Task scheduler.
 */

#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <semaphore.h>

typedef struct task_sched task_sched_t;

/**
 * @class task_sched
 * @author Juuso Alasuutari
 * @brief Task scheduler.
 */
struct task_sched {
	//! Write state
	struct {
		sem_t         q; //!< Write queue semaphore
		atomic_size_t p; //!< Non-masked write offset
	} w;

	//! Read state
	struct {
		sem_t         q; //!< Read queue semaphore
		atomic_size_t p; //!< Non-masked read offset
	} r;

	const size_t    task_u64; //!< Size of task entry as number of 64-bit blocks
	const size_t    buf_mask; //!< Bitmask for task buffer offset
	const uint64_t *buf;      //!< Pointer to task buffer
} __attribute__((packed,gcc_struct));

__attribute__((always_inline))
static inline bool
task_sched_init (task_sched_t *s,
                 const size_t  task_u64,
                 const size_t  buf_log2)
{
	const size_t buf_size = (const size_t)1 << buf_log2;
	const size_t buf_bytes = task_u64 << (buf_log2 + 3);
	printf ("%s: buf_size=%zu, buf_bytes=%zu\n", __func__, buf_size, buf_bytes);
	if ((s->buf = aligned_alloc (buf_bytes, buf_bytes))) {
		*(size_t *)&s->buf_mask = buf_size - 1;
		*(size_t *)&s->task_u64 = task_u64;
		atomic_init (&s->r.p, 0);
		if (sem_init (&s->r.q, 0, 0) == 0) {
			atomic_init (&s->w.p, 0);
			if (sem_init (&s->w.q, 0, buf_size) == 0) {
				return true;
			}
			fprintf (stderr, "%s: sem_init: %s\n",
			                 __func__, strerror (errno));
			(void) sem_destroy (&s->r.q);
		} else {
			fprintf (stderr, "%s: sem_init: %s\n",
			                 __func__, strerror (errno));
		}
		free ((void *) s->buf);
		s->buf = NULL;
	} else {
		fprintf (stderr, "%s: aligned_alloc: %s\n",
		                 __func__, strerror (errno));
	}
	return false;
}

__attribute__((always_inline))
static inline void
task_sched_fini (task_sched_t *s)
{
	atomic_init (&s->w.p, 0);
	(void) sem_destroy (&s->w.q);
	atomic_init (&s->r.p, 0);
	(void) sem_destroy (&s->r.q);
	*(size_t *)&s->task_u64 = 0;
	*(size_t *)&s->buf_mask = 0;
	free ((void *) s->buf);
	s->buf = NULL;
}

__attribute__((always_inline))
static inline void
_task_put (task_sched_t *s,
           uint64_t     *t)
{
	// get pointer to next writable task
	size_t p = atomic_fetch_add (&s->w.p, 1);
	uint64_t *_t = (uint64_t *) s->buf + ((p & s->buf_mask) * s->task_u64);
	printf ("%s: task entry %zu (%p)\n", __func__, p, (uint8_t *)_t);

	// copy task entry to task buffer
	for (size_t i = 0; i < s->task_u64; ++i) {
		_t[i] = t[i];
	}

	// wake up reader
	if (sem_post (&s->r.q)) {
		fprintf (stderr, "%s: sem_post: %s\n",
		                 __func__, strerror (errno));
	}
}

__attribute__((always_inline))
static inline bool
task_put (task_sched_t *s,
          uint64_t     *t)
{
	do {
		// wait until a task can be added
		printf ("%s: queuing for write\n", __func__);
		if (sem_wait (&s->w.q) == 0) {
			_task_put (s, t);
			return true;
		}
	} while (EINTR == errno); // retry after signal interrupt
	fprintf (stderr, "%s: sem_wait: %s\n",
	                 __func__, strerror (errno));
	return false;
}

__attribute__((always_inline))
static inline bool
task_put_nb (task_sched_t *s,
             uint64_t     *t)
{
	do {
		// test if a task can be added
		printf ("%s: queuing for write\n", __func__);
		if (sem_trywait (&s->w.q) == 0) {
			_task_put (s, t);
			return true;
		}
		if (EAGAIN == errno) {
			printf ("%s: queue would block\n", __func__);
			return false;
		}
	} while (EINTR == errno); // retry after signal interrupt
	fprintf (stderr, "%s: sem_trywait: %s\n",
	                 __func__, strerror (errno));
	return false;
}

__attribute__((always_inline))
static inline void
_task_get (task_sched_t *s,
           uint64_t     *t)
{
	// get pointer to next available queued task
	size_t p = atomic_fetch_add (&s->r.p, 1);
	uint64_t *_t = (uint64_t *) s->buf + ((p & s->buf_mask) * s->task_u64);
	printf ("%s: task entry %zu (%p)\n", __func__, p, (uint8_t *)_t);

	// copy task entry from task buffer
	for (size_t i = 0; i < s->task_u64; ++i) {
		t[i] = _t[i];
	}

	// release task entry for rewriting
	if (sem_post (&s->w.q)) {
		fprintf (stderr, "%s: sem_post: %s\n",
		                 __func__, strerror (errno));
	}
}

__attribute__((always_inline))
static inline bool
task_get (task_sched_t *s,
          uint64_t     *t)
{
	do {
		// wait until a task is available
		printf ("%s: queuing for read\n", __func__);
		if (sem_wait (&s->r.q) == 0) {
			_task_get (s, t);
			return true;
		}
	} while (EINTR == errno); // retry after signal interrupt
	fprintf (stderr, "%s: sem_wait: %s\n",
	                 __func__, strerror (errno));
	return false;
}

__attribute__((always_inline))
static inline bool
task_get_nb (task_sched_t *s,
             uint64_t     *t)
{
	do {
		// test if a task can be acquired
		printf ("%s: queuing for read\n", __func__);
		if (sem_trywait (&s->r.q) == 0) {
			_task_get (s, t);
			return true;
		}
		if (EAGAIN == errno) {
			printf ("%s: queue would block\n", __func__);
			return false;
		}
	} while (EINTR == errno); // retry after signal interrupt
	fprintf (stderr, "%s: sem_wait: %s\n",
	                 __func__, strerror (errno));
	return false;
}

#ifdef __cplusplus
}
#endif

#endif // __TASK_H__
