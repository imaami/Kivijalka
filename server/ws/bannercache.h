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
#ifndef BANNERCACHE_H
#define BANNERCACHE_H

#include <QtCore/QObject>

#include "cache.h"

class BannerCache : public QObject
{
	Q_OBJECT
public:
	explicit BannerCache(cache_t *cache,
	                     QObject *parent = Q_NULLPTR);
	~BannerCache();

private:
	cache_t *ptr;
};

#endif // BANNERCACHE_H
