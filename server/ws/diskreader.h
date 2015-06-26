#ifndef DISKREADER_H
#define DISKREADER_H

#include <QtCore/QThread>
#include "watcher.h"

class DiskReader : public QThread
{
    Q_OBJECT

public:
	DiskReader(watcher_t *watcher,
	           QObject   *parent = 0);
	~DiskReader();

protected:
	void run() Q_DECL_OVERRIDE;

private:
	watcher_t *watcher;
};

#endif // DISKREADER_H
