#ifndef IMGWORKER_H
#define IMGWORKER_H

#include <QtCore/QObject>

class ImgWorker : public QObject
{
	Q_OBJECT

public:
	ImgWorker(QObject *parent = 0);
	~ImgWorker();

public slots:
	void process();
};

#endif // IMGWORKER_H
