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
#ifndef DISKREADER_H
#define DISKREADER_H

#include <QtCore/QObject>
#include "watcher.h"

class DiskReader : public QObject
{
    Q_OBJECT

public:
	DiskReader(watcher_t *watcher,
	           QObject   *parent = 0);
	~DiskReader();

public slots:
	void process();

private:
	watcher_t *watcher;
};

#endif // DISKREADER_H
