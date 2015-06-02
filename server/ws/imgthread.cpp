#include "imgthread.h"
#include "global.h"
#include "img_file.h"
#include "img.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

ImgThread::ImgThread(QObject *parent) :
	QThread(parent)
{
}

ImgThread::~ImgThread()
{
}

void ImgThread::run()
{
	for (;;) {
		if (!sem_wait (&process_sem)) {
			unsigned int update_flags = 0;
			img_data_t *imd;
			if (img_file_steal_data (&capture_file, &imd)) {
				if (imd) {
					if (img_load_data (&img, 1, imd->data, imd->size)) {
						update_flags |= 1;
						std::printf ("%s: loaded capture data\n", __func__);
					} else {
						std::fprintf (stderr, "%s: img_load_data failed\n", __func__);
					}
					img_data_free (imd);
				} else {
					std::printf ("%s: no capture data available\n", __func__);
				}
			} else {
				std::fprintf (stderr, "%s: img_file_steal_data failed\n", __func__);
			}
			imd = NULL;
			switch (update_flags) {
			case 0:
				std::printf ("%s: nothing to update\n", __func__);
				continue;
			default:
				(void) img_file_post (&output_file);
			}
		} else if (errno != EINTR) {
			std::fprintf (stderr, "%s: sem_wait failed: %s\n",
			              __func__, std::strerror (errno));
		}
	}
}
