#ifndef IMGWORKER_H
#define IMGWORKER_H

#include <QtCore/QObject>

#include "display.h"

class ImgWorker : public QObject
{
	Q_OBJECT

public:
	ImgWorker(display_t *display,
	          QObject   *parent = 0);
	~ImgWorker();

public slots:
	void process();

private:
	display_t *display;
};

#endif // IMGWORKER_H
