#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <QtCore/QObject>

class DiskWriter : public QObject
{
    Q_OBJECT

public:
	DiskWriter(QObject *parent = 0);
	~DiskWriter();

public slots:
	void process();
};

#endif // DISKWRITER_H
