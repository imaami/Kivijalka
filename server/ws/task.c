/** \file task.c
 *
 * Task buffer.
 */

#include "task.h"

task_t task_buf[TASK_BUF_TASKS] __attribute__((aligned(TASK_BYTES)));
sem_t task_write_sem, task_read_sem;
volatile atomic_size_t task_write_ptr, task_read_ptr;
