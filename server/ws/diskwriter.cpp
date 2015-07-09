#include "diskwriter.h"
#include "global.h"
#include "img_data.h"
#include "img_file.h"

#include <cstdio>

DiskWriter::DiskWriter(const char *display_file_path,
                       QObject    *parent) :
	QObject(parent)
{
	display_file = file_create (display_file_path);
}

DiskWriter::~DiskWriter()
{
	file_destroy (&display_file);
}

void DiskWriter::process()
{
	if (!display_file) {
		std::fprintf (stderr, "DiskWriter::%s: file_create failed, "
		              "can't run thread\n", __func__);
		return;
	}

	for (;;) {
		if (img_file_wait (&output_file)) {
			img_data_t *imd;
//			std::printf ("diskwriter triggered\n");
			if ((imd = img_file_steal_data (&output_file))) {
				if (file_open (display_file, "wb")) {
					(void) file_write (display_file, imd->size, (const uint8_t *) imd->data);
					(void) file_close (display_file);
				}
				img_data_free (imd);
				imd = NULL;
			}
		}
	}
}
