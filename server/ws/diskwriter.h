#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <QtCore/QObject>
#include "file.h"

class DiskWriter : public QObject
{
	Q_OBJECT

public:
	DiskWriter(const char *display_file_path,
	           QObject    *parent = 0);
	~DiskWriter();

public slots:
	void process();

private:
	file_t *display_file;
};

#endif // DISKWRITER_H
