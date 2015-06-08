#include "diskwriter.h"
#include "global.h"
#include "img_data.h"
#include "img_file.h"

#include <QtCore/QFile>
#include <cstdio>

DiskWriter::DiskWriter(QObject *parent) :
	QThread(parent)
{
}

DiskWriter::~DiskWriter()
{
}

void DiskWriter::run()
{
	for (;;) {
		if (img_file_wait (&output_file)) {
			img_data_t *imd;
//			std::printf ("diskwriter triggered\n");
			if ((imd = img_file_steal_data (&output_file))) {
				QFile f(QString (output_file.path));
				if (f.open (QIODevice::WriteOnly)) {
					const char *p = (const char *) imd->data;
					qint64 n = (qint64) imd->size, x;
					while (n > 0) {
						if ((x = f.write (p, n)) < 0) {
							std::fprintf (stderr, "DiskWriter::%s: QFile::write failed\n", __func__);
							break;
						}
						if (x == n) {
//							std::printf ("DiskWriter::%s: wrote output data\n", __func__);
							break;
						}
						n -= x;
						p += x;
					}
					f.close ();
				}
				img_data_free (imd);
				imd = NULL;
			}
		}
	}
}
