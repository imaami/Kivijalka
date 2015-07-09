#include <cstdio>
#include <semaphore.h>
#include "imgworker.h"
#include "global.h"
#include "img_data.h"
#include "img_file.h"

#include <QtGui/QImage>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtCore/QPoint>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>

ImgWorker::ImgWorker(display_t *display,
                     QObject   *parent) :
	QObject(parent)
{
	this->display = display;
}

ImgWorker::~ImgWorker()
{
	this->display = NULL;
}

/*
inline bool ImgWorker::img_save(QImage  &img,
                                uint8_t *buf,
                                size_t  *size)
{
	std::printf ("ImgWorker::%s\n", __func__);
	return true;
}
*/

__attribute__((always_inline))
static inline const char *
fmtstr (enum QImage::Format fmt)
{
	switch (fmt) {
	case QImage::Format_Invalid:
		return "Invalid";
	case QImage::Format_Mono:
		return "Mono";
	case QImage::Format_MonoLSB:
		return "MonoLSB";
	case QImage::Format_Indexed8:
		return "Indexed8";
	case QImage::Format_RGB32:
		return "RGB32";
	case QImage::Format_ARGB32:
		return "ARGB32";
	case QImage::Format_ARGB32_Premultiplied:
		return "ARGB32_Premultiplied";
	case QImage::Format_RGB16:
		return "RGB16";
	case QImage::Format_ARGB8565_Premultiplied:
		return "ARGB8565_Premultiplied";
	case QImage::Format_RGB666:
		return "RGB666";
	case QImage::Format_ARGB6666_Premultiplied:
		return "ARGB6666_Premultiplied";
	case QImage::Format_RGB555:
		return "RGB555";
	case QImage::Format_ARGB8555_Premultiplied:
		return "ARGB8555_Premultiplied";
	case QImage::Format_RGB888:
		return "RGB888";
	case QImage::Format_RGB444:
		return "RGB444";
	case QImage::Format_ARGB4444_Premultiplied:
		return "ARGB4444_Premultiplied";
	case QImage::Format_RGBX8888:
		return "RGBX8888";
	case QImage::Format_RGBA8888:
		return "RGBA8888";
	case QImage::Format_RGBA8888_Premultiplied:
		return "RGBA8888_Premultiplied";
	case QImage::Format_BGR30:
		return "BGR30";
	case QImage::Format_A2BGR30_Premultiplied:
		return "A2BGR30_Premultiplied";
	case QImage::Format_RGB30:
		return "RGB30";
	case QImage::Format_A2RGB30_Premultiplied:
		return "A2RGB30_Premultiplied";
	default:
		break;
	}
	return "unknown";
}

void ImgWorker::process()
{
	if (!display) {
		std::fprintf (stderr, "ImgWorker::%s: null display, "
		              "can't run thread\n", __func__);
		return;
	}

	int dw = display_width (display),
	    dh = display_height (display);

	if (dw < 1 || dh < 1) {
		std::fprintf (stderr,
		              "ImgWorker::%s: illegal display geometry\n",
		              __func__);
		return;
	}

	int dx = 0, dy = 0,
	    bw = 0, bh = 0,
	    bx = 0, by = 0;
	enum QImage::Format fmt = QImage::Format_ARGB32_Premultiplied;
	QImage capture(dw, dh, fmt);
	QImage banner;

	capture.fill (QColor (0, 0, 0, 255));

	for (;;) {
		if (sem_wait (&process_sem)) {
			continue;
		}

		img_data_t *cd = img_file_steal_data (&capture_file);
		img_data_t *bd = img_file_steal_data (&banner_file);

		if (cd) {
			QImage c;
			if (c.loadFromData ((const uchar *) cd->data,
			                    (int) cd->size)) {
				int cw = c.width(), ch = c.height();
				if (cw < 1 || ch < 1) {
					goto capture_load_failed;
				}
				if (cw != dw || ch != dh) {
					c = c.scaled (dw, dh,
					              Qt::KeepAspectRatio,
					              Qt::SmoothTransformation);
					cw = c.width();
					ch = c.height();
					if (cw < 1 || ch < 1) {
						goto capture_load_failed;
					}
					std::printf ("ImgWorker::%s: scaled capture image\n", __func__);
				}
				dx = (cw < dw) ? ((dw - cw) & ~1) >> 1 : 0;
				dy = (ch < dh) ? ((dh - ch) & ~1) >> 1 : 0;
				c.setOffset (QPoint (dx, dy));
				QPainter pr(&capture);
//				pr.drawImage();
			} else {
			capture_load_failed:
				c = QImage();
				std::fprintf (stderr, "ImgWorker::%s: can't "
				              "load captured image, filling "
				              "with empty pixels instead\n",
				              __func__);
			fill_capture:
				capture.fill (QColor (0, 0, 0, 255));
			}

			std::printf ("ImgWorker::%s: capture width=%d height=%d depth=%d format=%s\n",
			             __func__, capture.width(), capture.height(),
			             capture.depth(), fmtstr (capture.format()));

			if (bd) {
				goto have_banner;
			} else {
				goto do_render;
			}
		} else if (bd) {
		have_banner:
			if (banner.loadFromData ((const uchar *) bd->data,
			                         (int) bd->size)) {
				if (banner.format() != QImage::Format_ARGB32) {
					std::printf ("ImgWorker::%s: converting banner format from %s to %s\n",
					             __func__, fmtstr (banner.format()), fmtstr (QImage::Format_ARGB32));
					banner = banner.convertToFormat (QImage::Format_ARGB32);
				}
			}
		do_render:
			std::printf ("render\n");
		} else {
			continue;
		}

/*
		QByteArray bar;
		QBuffer buf(&bar);
			if (buf.open (QIODevice::WriteOnly)) {
				if (img.save (&buf, "PNG")) {
					
				}
		}
*/




//			img_file_replace_data (&output_file, imd);
//			(void) img_file_post (&output_file);

		if (cd) {
			img_data_free (cd);
			cd = NULL;
		}
		if (bd) {
			img_data_free (bd);
			bd = NULL;
		}
	}
}
