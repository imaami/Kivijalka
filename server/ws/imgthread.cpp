#include "imgthread.h"
#include "global.h"
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
			if (img_load_data (&img, 1, capture_data, capture_size)) {
				std::printf ("loaded capture data\n");
				if (sem_post (&output_sem)) {
					std::fprintf (stderr, "%s: sem_post failed: %s\n",
					              __func__, std::strerror (errno));
				}
			}
		} else if (errno != EINTR) {
			std::fprintf (stderr, "%s: sem_wait failed: %s\n",
			              __func__, std::strerror (errno));
		}
	}
}
