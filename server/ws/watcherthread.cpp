#include "watcherthread.h"

WatcherThread::WatcherThread(const QString &path, QObject *parent)
    : QThread(parent)
{
	abort = (!(watcher = watcher_create (path.toUtf8().data())));
}

WatcherThread::~WatcherThread()
{
	mutex.lock();
	abort = true;
	condition.wakeOne();
	mutex.unlock();

	wait();

	watcher_destroy (watcher);
	watcher = NULL;
}

void WatcherThread::run()
{
	while (!abort) {
		int64_t i = watcher_run_once (watcher);
		if (i > 0) {
			if ((i & 1)) {
				emit fileUpdated();
			}
			if ((i & 2)) {
				emit capFileUpdated();
			}
		}
	}
/*
        mutex.lock();
        QSize resultSize = this->resultSize;
        double scaleFactor = this->scaleFactor;
        double centerX = this->centerX;
        double centerY = this->centerY;
        mutex.unlock();
*/
/*
        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
*/
}
