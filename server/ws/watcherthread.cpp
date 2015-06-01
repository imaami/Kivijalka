#include "watcherthread.h"
#include "global.h"
#include "img.h"

#include <cstdio>
#include <cerrno>
#include <cstring>

WatcherThread::WatcherThread(QObject *parent)
    : QThread(parent)
{
	watcher = watcher_create (capture_file.path);
}

WatcherThread::~WatcherThread()
{
	watcher_destroy (watcher);
	watcher = NULL;
}

void WatcherThread::run()
{
	for (;;) {
		if (watcher_run_once (watcher) > 0) {
			(void) img_file_post (&capture_file);
		}
	}
}
