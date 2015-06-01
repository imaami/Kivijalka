/** \file task_buf.h
 *
 * Task buffer.
 */

#ifndef __TASK_BUF_H__
#define __TASK_BUF_H__

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

typedef struct task task_t;

#define TASK_POW (6)
#define TASK_BUF_POW (4)
#define TASK_BYTES ((size_t)1 << TASK_POW)
#define TASK_BUF_BYTES (TASK_BYTES << TASK_BUF_POW)
#define TASK_BUF_TASKS ((size_t)1 << TASK_BUF_POW)
#define TASK_PTR_MASK (TASK_BUF_TASKS - 1)

struct task {
	union {
		uint64_t u64[TASK_BYTES >> 3];
		uint32_t u32[TASK_BYTES >> 2];
		uint16_t u16[TASK_BYTES >> 1];
		uint8_t  u8[TASK_BYTES];
	};
} __attribute__((packed,gcc_struct,aligned(TASK_BYTES)));

typedef void (*task_func) (task_t *ctx);

extern task_t task_buf[TASK_BUF_TASKS] __attribute__((aligned(TASK_BYTES)));
extern sem_t task_write_sem, task_read_sem;
extern volatile atomic_size_t task_write_ptr, task_read_ptr;

__attribute__((always_inline))
static inline void
task_buf_init (void)
{
	if (sem_init (&task_write_sem, 0, TASK_BUF_TASKS)
	    || sem_init (&task_read_sem, 0, 0)) {
		fprintf (stderr, "%s: sem_init failed: %s\n",
		                 __func__, strerror (errno));
		abort ();
	}
	task_write_ptr = task_read_ptr = 0;
	memset ((void *) task_buf, 0, TASK_BUF_BYTES);
}

__attribute__((always_inline))
static inline void
task_buf_fini (void)
{
	(void) sem_destroy (&task_write_sem);
	(void) sem_destroy (&task_read_sem);
}

__attribute__((always_inline))
static inline size_t
task_ptr_advance (volatile atomic_size_t *ptr)
{
	atomic_size_t cur;
	do {
		cur = atomic_load (ptr);
	} while (!atomic_compare_exchange_weak (ptr, &cur, cur + 1));
	return (cur & TASK_PTR_MASK);
}

__attribute__((always_inline))
static inline bool
task_wait (sem_t *task_sem)
{
	for (;;) {
		if (!sem_wait (task_sem)) {
			return true;
		} else if (errno != EINTR) {
			fprintf (stderr, "%s: sem_wait failed: %s\n",
			                 __func__, strerror (errno));
			return false;
		}
	}
}

__attribute__((always_inline))
static inline task_t *
task_wait_write (void)
{
	return (task_wait (&task_write_sem))
	       ? &task_buf[task_ptr_advance (&task_write_ptr)]
	       : NULL;
}

__attribute__((always_inline))
static inline task_t *
task_wait_read (void)
{
	return (task_wait (&task_read_sem))
	       ? &task_buf[task_ptr_advance (&task_read_ptr)]
	       : NULL;
}

__attribute__((always_inline))
static inline bool
task_post (sem_t *task_sem)
{
	if (sem_post (task_sem)) {
		fprintf (stderr, "%s: sem_post failed: %s\n",
		                 __func__, strerror (errno));
		return false;
	}
	return true;
}

__attribute__((always_inline))
static inline void
task_post_write (void)
{
	(void) task_post (&task_write_sem);
}

__attribute__((always_inline))
static inline void
task_post_read (void)
{
	(void) task_post (&task_read_sem);
}

#ifdef __cplusplus
}
#endif

#endif // __TASK_BUF_H__
