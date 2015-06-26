#include <semaphore.h>
#include "imgworker.h"
#include "global.h"
#include "img_file.h"

ImgWorker::ImgWorker(QObject *parent) :
	QObject(parent)
{
}

ImgWorker::~ImgWorker()
{
}

void ImgWorker::process()
{
	for (;;) {
		if (!sem_wait (&process_sem)) {
			(void) img_file_post (&output_file);
		}
	}
}
