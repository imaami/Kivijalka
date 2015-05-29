#ifndef WATCHERTHREAD_H
#define WATCHERTHREAD_H

#include <QtCore/QThread>
#include "watcher.h"

class WatcherThread : public QThread
{
    Q_OBJECT

public:
	WatcherThread(QObject *parent = 0);
	~WatcherThread();

protected:
	void run() Q_DECL_OVERRIDE;

private:
	watcher_t *watcher;
};

#endif // WATCHERTHREAD_H
