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
#include "task.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int
main (int    argc,
      char **argv)
{
	task_sched_t sched;

	if (!task_sched_init (&sched, 4, 2)) {
		fprintf (stderr, "%s: task_sched_init failed\n", __func__);
		return EXIT_FAILURE;
	}

	printf ("task_sched.task_u64: %zu\ntask_sched.buf_mask: %zu\ntask_sched.buf: %p\n",
	        sched.task_u64, sched.buf_mask, (uint8_t *)sched.buf);

	uint64_t task[4] = {0, 1, 2, 3},
	         task2[4] = {0, 0, 0, 0};

	for (unsigned int i = 0; i < 1024; ++i) {
		while (task_put_nb (&sched, task)) {
			++task[0]; ++task[1]; ++task[2]; ++task[3];
		}
		while (task_get_nb (&sched, task2)) {
			printf ("-> {%lu, %lu, %lu, %lu}\n",
			        task2[0], task2[1], task2[2], task2[3]);
		}
	}

	task_sched_fini (&sched);

	return EXIT_SUCCESS;
}
