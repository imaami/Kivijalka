#include "diskreader.h"
#include "read-file.h"
#include "global.h"
#include "img.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

DiskReader::DiskReader(QObject *parent) :
	QThread(parent)
{
}

DiskReader::~DiskReader()
{
}

void DiskReader::run()
{
	for (;;) {
		if (img_file_wait (&capture_file)) {
			std::printf ("disk reader triggered\n");
			if (img_file_read_data (&capture_file)) {
				std::printf ("read capture\n");
				if (capture_file.size > 0) {
					if (sem_post (&process_sem)) {
						std::fprintf (stderr, "%s: sem_post failed: %s\n",
						              __func__, std::strerror (errno));
					}
				} else {
					std::fprintf (stderr, "%s: capture file is empty\n", __func__);
				}
			}
		}
	}
}
