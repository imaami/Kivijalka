#include "watcherthread.h"
#include "global.h"

#include <cstdio>
#include <cerrno>
#include <cstring>

WatcherThread::WatcherThread(QObject *parent)
    : QThread(parent)
{
	watcher = watcher_create (capture_file);
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
			if (sem_post (&capture_sem)) {
				std::fprintf (stderr, "%s: sem_post failed: %s\n",
				              __func__, std::strerror (errno));
			}
		}
	}
}
