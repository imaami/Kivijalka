#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
#include "global.h"
#include "display.h"
#include "img_file.h"
#include "watcher.h"
#include "diskreader.h"
#include "imgworker.h"
#include "diskwriter.h"
#include "wsserver.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	global_init (1024, 768, 640, 512);
	(void) img_file_set_path (&capture_file, "/dev/shm/kivijalka/cap-0510.png");
	(void) img_file_set_path (&output_file, "/dev/shm/kivijalka/out-0510.png");

	display_t *d;
	if (!(d = display_create (1280, 1024))) {
		return -1;
	}

	watcher_t *w;
	if (!(w = watcher_create (capture_file.path))) {
	fail:
		display_destroy (d);
		d = NULL;
		global_fini ();
		return -1;
	}

	QThread diskReaderThread;
	DiskReader diskReader (w);

	QThread imgWorkerThread;
	ImgWorker imgWorker;

	QThread diskWriterThread;
	DiskWriter diskWriter(output_file.path);

	if (!watcher_prepare (w)) {
		watcher_destroy (w);
		w = NULL;
		goto fail;
	}

	diskReader.moveToThread (&diskReaderThread);
	QObject::connect (&diskReaderThread, &QThread::started,
	                  &diskReader, &DiskReader::process);

	imgWorker.moveToThread (&imgWorkerThread);
	QObject::connect (&imgWorkerThread, &QThread::started,
	                  &imgWorker, &ImgWorker::process);

	diskWriter.moveToThread (&diskWriterThread);
	QObject::connect (&diskWriterThread, &QThread::started,
	                  &diskWriter, &DiskWriter::process);

	diskWriterThread.start ();
	imgWorkerThread.start ();
	diskReaderThread.start ();
	watcher_start (w);

	WSServer *server = new WSServer(8001, 1280, 1024, 640, 512, 1024, 768,
	                                QString("/usr/share/kivijalka/banners"));
	QObject::connect(server, &WSServer::closed, &a, &QCoreApplication::quit);

	int r = a.exec();

	watcher_stop (w);
	watcher_destroy (w);
	w = NULL;

	display_destroy (d);
	d = NULL;

	global_fini ();

	return r;
}
