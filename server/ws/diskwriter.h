#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <QtCore/QThread>

class DiskWriter : public QThread
{
    Q_OBJECT

public:
	DiskWriter(QObject *parent = 0);
	~DiskWriter();

protected:
	void run() Q_DECL_OVERRIDE;
};

#endif // DISKWRITER_H
