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

__attribute__((always_inline))
static inline void
update_capture (QImage     *capture,
                img_data_t *img,
                int         dw,
                int         dh,
                QColor     &bgcolor,
                QPainter   *pr)
{
	QImage c;

	if (c.loadFromData ((const uchar *) img->data, (int) img->size)) {
		int cw = c.width();
		int ch = c.height();

		if (cw < 1 || ch < 1) {
			goto capture_load_failed;
		}

		std::printf ("ImgWorker::%s: loaded new capture: "
		             "w=%d h=%d fmt=%s\n", __func__,
		             c.width(), c.height(), fmtstr (c.format()));

		if (cw != dw || ch != dh) {
			c = c.scaled (dw, dh,
			              Qt::KeepAspectRatio,
			              Qt::SmoothTransformation);
			cw = c.width();
			ch = c.height();
			if (cw < 1 || ch < 1) {
				goto capture_load_failed;
			}
			std::printf ("ImgWorker::%s: scaled capture to %dx%d\n",
			             __func__, cw, ch);
		}

		int dx = (cw < dw) ? ((dw - cw) & ~1) >> 1 : 0;
		int dy = (ch < dh) ? ((dh - ch) & ~1) >> 1 : 0;

		if (dx > 0) {
			pr->setCompositionMode (QPainter::CompositionMode_Source);
			pr->fillRect (0, 0, dx, dh, bgcolor);
			pr->fillRect (dx + cw, 0, dw - (dx + cw), dh, bgcolor);
			if (dy > 0) {
				goto draw_horiz_border;
			}
		} else if (dy > 0) {
			pr->setCompositionMode (QPainter::CompositionMode_Source);
		draw_horiz_border:
			pr->fillRect (0, 0, dw, dy, bgcolor);
			pr->fillRect (0, dy + ch, dw, dh - (dy + ch), bgcolor);
		}

		pr->setCompositionMode (QPainter::CompositionMode_SourceOver);
		pr->drawImage (dx, dy, c);
	} else {
	capture_load_failed:
		capture->fill (bgcolor);
		std::fprintf (stderr, "ImgWorker::%s: couldn't load capture "
		              "image, filled with background color instead\n",
		              __func__);
	}

	c = QImage();
}

__attribute__((always_inline))
static inline void
update_banner (QImage              *banner,
               img_data_t          *img,
               enum QImage::Format  fmt)
{
	QImage b;

	if (b.loadFromData ((const uchar *) img->data, (int) img->size)) {
		std::printf ("ImgWorker::%s: loaded new banner: "
		             "w=%d h=%d fmt=%s\n", __func__,
		             b.width(), b.height(), fmtstr (b.format()));

		if (b.format() == fmt) {
			*banner = b.copy();
		} else {
			std::printf ("ImgWorker::%s: converting banner format to %s\n",
			             __func__, fmtstr (fmt));
			*banner = b.convertToFormat (fmt);
		}
	} else {
		std::fprintf (stderr, "ImgWorker::%s: couldn't load banner, "
		              "keeping old one\n", __func__);
	}

	b = QImage();
}

__attribute__((always_inline))
static inline void
update_display (QImage              &capture,
                QImage              &banner,
                display_t           *d,
                int                  dw,
                int                  dh,
                enum QImage::Format  fmt)
{
	bool r;
	uint32_t *c = (uint32_t *) capture.constBits();
	if (!banner.isNull()) {
		uint32_t *b = (uint32_t *) banner.constBits();
		uint32_t bw = (uint32_t) banner.width();
		uint32_t bh = (uint32_t) banner.height();
		uint32_t bx = capture.width() - bw;
		uint32_t by = capture.height() - bh;
		r = display_render (d, c, b, bw, bh, bx, by);
	} else {
		r = display_render_bg (d, c);
	}
	if (r) {
		QImage img((const uchar *) display_pixbuf (d), dw, dh, fmt);
		QByteArray bar;
		QBuffer buf(&bar);
		if (buf.open (QIODevice::WriteOnly)) {
			if (img.save (&buf, "PNG")) {
				img_data_t *imd;
				if ((imd = img_data_new_from_buffer ((size_t) bar.size(), bar.constData()))) {
					img_file_replace_data (&output_file, imd);
					(void) img_file_post (&output_file);
				}
			}
			buf.close();
		}
	}
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

	QColor bgcolor(0, 0, 0, 255);
	enum QImage::Format fmt = QImage::Format_ARGB32_Premultiplied;
	QImage capture(dw, dh, fmt);
	QImage banner;
	capture.fill (bgcolor);
	QPainter pr(&capture);

	for (;;) {
		if (sem_wait (&process_sem)) {
			continue;
		}

		img_data_t *cd = img_file_steal_data (&capture_file);
		img_data_t *bd = img_file_steal_data (&banner_file);

		if (cd) {
			update_capture (&capture, cd, dw, dh, bgcolor, &pr);

			if (bd) {
				goto have_banner;
			}

		} else if (bd) {
		have_banner:
			update_banner (&banner, bd, fmt);

		} else {
			continue;
		}

		update_display (capture, banner, display, dw, dh, fmt);

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
