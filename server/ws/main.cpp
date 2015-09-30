#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QDir>

#include "global.h"
#include "display.h"
#include "img_file.h"
#include "watcher.h"
#include "diskreader.h"
#include "imgworker.h"
#include "diskwriter.h"
#include "wsserver.h"
#include "banner.h"
#include "cache.h"
#include "args.h"

int main(int argc, char *argv[])
{
	if (!args_parse (argc, argv)) {
		fprintf (stderr, "%s: error parsing command line arguments\n",
		         __func__);
		return -9;
	}

	uint32_t cw = args_get_capture_width(), ch = args_get_capture_height(),
	         dw = args_get_display_width(), dh = args_get_display_height(),
	         tw, th;
	const char *bp = args_get_cache_path();
	const char *uu = args_get_cache_uuid();
	const char *cp = args_get_capture_path();
	const char *op = args_get_output_path();
	const char *sa = args_get_server_addr();
	uint16_t sp = args_get_server_port();

	bool required_params = true;

	if (!cp) {
		fprintf (stderr, "%s: missing capture file path\n", __func__);
		required_params = false;
	}

	if (!op) {
		fprintf (stderr, "%s: missing output file path\n", __func__);
		required_params = false;
	}

	if (dw < 2 || dh < 2) {
		fprintf (stderr, "%s: invalid display size: %ux%u\n",
		         __func__, dw, dh);
		required_params = false;
	}

	if (!required_params) {
		printf ("Shamefur dispray!\n");
		return -8;
	}

	if (cw < 1 || ch < 1) {
		cw = dw;
		ch = dh;
	}

	tw = (cw & 0xfffffffe) >> 1;
	th = (ch & 0xfffffffe) >> 1;

	int _argc = 1;
	char *_argv[] = {argv[0], NULL};
	QCoreApplication a(_argc, _argv);

	global_init (tw, th);

	QString str(cp);
	QDir dir(str);
	(void) img_file_set_path (&capture_file, dir.absolutePath().toUtf8().data());
	str = QString(op);
	dir = QDir(str);
	(void) img_file_set_path (&output_file, dir.absolutePath().toUtf8().data());

	cache_t *c;
	str = QString(bp);
	dir = QDir(str);
	if (!(c = cache_create (dir.absolutePath().toUtf8().data()))) {
		global_fini ();
		return -7;
	}

	cache_import (c);

	if (!cur_banner && uu) {
		banner_t *b;
		if ((b = cache_find_banner_by_uuid_str (c, uu))) {
			if (!cache_activate_banner (c, b)) {
				fprintf (stderr,
				         "%s: failed to activate banner\n",
				         __func__);
			}
		}
	}

	display_t *d;
	if (!(d = display_create (cw, ch, dw, dh))) {
		cache_destroy (&c);
		global_fini ();
		return -6;
	}

	watcher_t *w;
	if (!(w = watcher_create (capture_file.path))) {
		display_destroy (d);
		d = NULL;
		cache_destroy (&c);
		global_fini ();
		return -5;
	}

	QThread diskReaderThread;
	DiskReader diskReader (w);

	QThread imgWorkerThread;
	ImgWorker imgWorker (d);

	QThread diskWriterThread;
	DiskWriter diskWriter(output_file.path);

	if (!watcher_prepare (w)) {
		watcher_destroy (w);
		w = NULL;
		display_destroy (d);
		d = NULL;
		cache_destroy (&c);
		global_fini ();
		return -4;
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

	WSServer *server = new WSServer(sa, sp, cw, ch, dw, dh, tw, th, c);
	if (!server) {
		watcher_destroy (w);
		w = NULL;
		display_destroy (d);
		d = NULL;
		cache_destroy (&c);
		global_fini ();
		return -3;
	}

	QObject::connect (&imgWorker, &ImgWorker::thumbnailUpdated,
	                  server, &WSServer::thumbnailUpdated,
	                  Qt::QueuedConnection);
	QObject::connect(server, &WSServer::closed, &a, &QCoreApplication::quit);

	diskWriterThread.start ();
	imgWorkerThread.start ();
	diskReaderThread.start ();
	watcher_start (w);

	int r = (server->listen())
	        ? (a.exec() == 0) ? 0 : -1
	        : -2;

	watcher_stop (w);
	delete (server);
	server = NULL;
	watcher_destroy (w);
	w = NULL;

	display_destroy (d);
	d = NULL;

	cache_destroy (&c);

	global_fini ();

	return r;
}
