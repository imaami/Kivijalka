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
#include "diskreader.h"
#include "global.h"
#include "img_file.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

DiskReader::DiskReader(watcher_t *watcher,
                       QObject   *parent) :
	QObject(parent)
{
	this->watcher = watcher;
}

DiskReader::~DiskReader()
{
}

void DiskReader::process()
{
	for (;;) {
		if (watcher_wait (watcher)) {
//			std::printf ("disk reader triggered\n");
			if (!img_file_update (&capture_file)) {
				std::fprintf (stderr, "%s: img_file_update failed\n", __func__);
			} else if (sem_post (&process_sem)) {
				std::fprintf (stderr, "%s: sem_post failed: %s\n",
				              __func__, std::strerror (errno));
			}
		}
	}
}
