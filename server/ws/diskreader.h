#ifndef DISKREADER_H
#define DISKREADER_H

#include <QtCore/QThread>

class DiskReader : public QThread
{
    Q_OBJECT

public:
	DiskReader(QObject *parent = 0);
	~DiskReader();

protected:
	void run() Q_DECL_OVERRIDE;
};

#endif // DISKREADER_H
