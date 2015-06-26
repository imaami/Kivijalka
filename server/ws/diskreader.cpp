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
