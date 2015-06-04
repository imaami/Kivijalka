#include "imgthread.h"
#include "global.h"
#include "img.h"

ImgThread::ImgThread(QObject *parent) :
	QThread(parent)
{
}

ImgThread::~ImgThread()
{
}

void ImgThread::run()
{
	img_thread (&img,
	            banner_x, banner_y,
	            thumb_w, thumb_h,
	            &process_sem,
	            &capture_file, &banner_file,
	            &output_file, &thumb_file);
}
