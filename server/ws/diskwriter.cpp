#include "diskwriter.h"
#include "global.h"
#include "img_data.h"
#include "img_file.h"
#include "file.h"

#include <cstdio>

DiskWriter::DiskWriter(QObject *parent) :
	QObject(parent)
{
}

DiskWriter::~DiskWriter()
{
}

void DiskWriter::process()
{
	for (;;) {
		if (img_file_wait (&output_file)) {
			img_data_t *imd;
//			std::printf ("diskwriter triggered\n");
			if ((imd = img_file_steal_data (&output_file))) {
				if (!file_write (output_file.path, imd->size,
				                 (const uint8_t *) imd->data)) {
					std::fprintf (stderr, "DiskWriter::%s: file_write failed\n", __func__);
				}
				img_data_free (imd);
				imd = NULL;
			}
		}
	}
}
