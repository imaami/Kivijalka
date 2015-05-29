#ifndef IMGTHREAD_H
#define IMGTHREAD_H

#include <QtCore/QThread>

class ImgThread : public QThread
{
    Q_OBJECT

public:
	ImgThread(QObject *parent = 0);
	~ImgThread();

protected:
	void run() Q_DECL_OVERRIDE;
};

#endif // IMGTHREAD_H
