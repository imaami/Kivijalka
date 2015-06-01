#include "diskwriter.h"
#include "global.h"
#include "img.h"

#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <cstring>

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
			std::printf("diskwriter triggered\n");
/*			if (output.open(QIODevice::WriteOnly)) {
				(void) output.write((const char *) *(this->data), *this->size);
				output.close();
				std::printf("wrote output data\n");
			}*/
		}
	}
}
