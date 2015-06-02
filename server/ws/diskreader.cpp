#include "diskreader.h"
#include "global.h"
#include "img_data.h"
#include "img_file.h"

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

			img_data_t *imd;

			if (!(imd = img_data_new_from_file (capture_file.path))) {
				std::fprintf (stderr, "%s: failed to read capture file\n", __func__);
				continue;
			}

			if (imd->size < 1) {
				std::fprintf (stderr, "%s: capture file empty\n", __func__);
				goto fail_free_data;
			}

			if (!img_file_replace_data (&capture_file, imd)) {
				std::fprintf (stderr, "%s: failed to replace capture data\n", __func__);
			fail_free_data:
				img_data_free (imd);
			} else if (sem_post (&process_sem)) {
				std::fprintf (stderr, "%s: sem_post failed: %s\n",
				              __func__, std::strerror (errno));
			}

			imd = NULL;
		}
	}
}
