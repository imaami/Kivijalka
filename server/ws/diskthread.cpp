#include "diskthread.h"

DiskThread::DiskThread(QObject *parent)
    : QThread(parent)
{
	abort = false;
}

DiskThread::~DiskThread()
{
	mutex.lock();
	abort = true;
	condition.wakeOne();
	mutex.unlock();

	wait();
}

void DiskThread::run()
{
	while (!abort) {
	}
/*
        mutex.lock();
        QSize resultSize = this->resultSize;
        double scaleFactor = this->scaleFactor;
        double centerX = this->centerX;
        double centerY = this->centerY;
        mutex.unlock();
*/
/*
        mutex.lock();
        if (!restart)
            condition.wait(&mutex);
        restart = false;
        mutex.unlock();
*/
}
