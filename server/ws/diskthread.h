#ifndef DISKTHREAD_H
#define DISKTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QString>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

class DiskThread : public QThread
{
    Q_OBJECT

public:
	DiskThread(QObject *parent = 0);
	~DiskThread();

//signals:
//	void fileUpdated();

protected:
	void run() Q_DECL_OVERRIDE;

private:
	QMutex mutex;
	QWaitCondition condition;
	bool abort;
};

#endif // DISKTHREAD_H
