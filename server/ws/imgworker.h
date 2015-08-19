#ifndef IMGWORKER_H
#define IMGWORKER_H

#include <QtCore/QObject>
#include <QtGui/QImage>

#include "display.h"

class ImgWorker : public QObject
{
	Q_OBJECT

public:
	ImgWorker(display_t *display,
	          QObject   *parent = 0);
	~ImgWorker();

signals:
	void thumbnailUpdated();

public slots:
	void process();

private:
	display_t *display;
	inline void update_display(QImage              &capture,
                                  QImage              &banner,
                                  int                  dw,
                                  int                  dh,
                                  int32_t              bx,
                                  int32_t              by,
                                  enum QImage::Format  fmt);
};

#endif // IMGWORKER_H
