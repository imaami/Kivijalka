/* Copyright © 2015 Koneet Kiertoon and Juuso Alasuutari.
 * Written by Juuso Alasuutari.
 *
 * This file is part of Kivijalka.
 *
 * Kivijalka is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Kivijalka is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kivijalka.  If not, see <http://www.gnu.org/licenses/>.
 */
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
