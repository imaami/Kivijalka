#ifndef DISKREADER_H
#define DISKREADER_H

#include <QtCore/QObject>
#include "watcher.h"

class DiskReader : public QObject
{
    Q_OBJECT

public:
	DiskReader(watcher_t *watcher,
	           QObject   *parent = 0);
	~DiskReader();

public slots:
	void process();

private:
	watcher_t *watcher;
};

#endif // DISKREADER_H
