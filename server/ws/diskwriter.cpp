#include "diskwriter.h"
#include "global.h"

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
		if (!sem_wait (&output_sem)) {
			std::printf("diskwriter triggered\n");
/*			if (output.open(QIODevice::WriteOnly)) {
				(void) output.write((const char *) *(this->data), *this->size);
				output.close();
				std::printf("wrote output data\n");
			}*/
		} else if (errno != EINTR) {
			std::fprintf (stderr, "%s: sem_wait failed: %s\n",
			              __func__, std::strerror (errno));
		}
	}
}
