#include "diskreader.h"
#include "read-file.h"
#include "global.h"

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
		if (!sem_wait (&capture_sem)) {
			std::printf ("disk reader triggered\n");
			if (capture_data) {
				std::free (capture_data);
			}
			if ((capture_data = read_binary_file (capture_file, &capture_size))) {
				std::printf ("read capture\n");
				if (capture_size > 0) {
					if (sem_post (&process_sem)) {
						std::fprintf (stderr, "%s: sem_post failed: %s\n",
						              __func__, std::strerror (errno));
					}
				} else {
					std::free (capture_data);
					capture_data = NULL;
					std::fprintf (stderr, "%s: capture file is empty\n", __func__);
				}
			} else {
				capture_size = 0;
				std::fprintf (stderr, "%s: read_binary_file failed\n", __func__);
			}
		} else if (errno != EINTR) {
			std::fprintf (stderr, "%s: sem_wait failed: %s\n",
			              __func__, std::strerror (errno));
		}
	}
}
