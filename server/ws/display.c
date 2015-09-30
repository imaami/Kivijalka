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
/** \file display.c
 *
 * Graphical display.
 */


#include "file.h"
#include "global.h"

#include "private/display.h"
#include "private/banner.h"
#include "private/hex.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

__attribute__((always_inline))
static inline bool
_display_init (struct display *d,
               const uint32_t  width,
               const uint32_t  height,
               const uint32_t  scaled_w,
               const uint32_t  scaled_h)
{
	size_t buf_bytes = (width * height) << 2;

	if (!(d->pixbuf = aligned_alloc (16, buf_bytes))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		return false;
	}

	d->width = width;
	d->height = height;
	if (scaled_w > 0 && scaled_h > 0) {
		d->scaled_w = scaled_w;
		d->scaled_h = scaled_h;
	} else {
		d->scaled_w = width;
		d->scaled_h = height;
	}

	(void) memset ((void *) d->pixbuf, 0, buf_bytes);

	return true;
}

__attribute__((always_inline))
static inline void
_display_fini (struct display *d)
{
	if (d->pixbuf) {
		(void) memset ((void *) d->pixbuf, 0,
		               (d->width * d->height) << 2);
		free (d->pixbuf);
		d->pixbuf = NULL;
	}
	d->width = 0;
	d->height = 0;
}

struct display *
display_create (const uint32_t width,
                const uint32_t height,
                const uint32_t scaled_w,
                const uint32_t scaled_h)
{
	if (width < 1 || height < 1 || (SIZE_MAX / 4) / width < height) {
		fprintf (stderr, "%s: invalid arguments\n", __func__);
		return NULL;
	}

	struct display *d;

	if (!(d = aligned_alloc (16, sizeof (struct display)))) {
		fprintf (stderr, "%s: aligned_alloc failed\n", __func__);
		return NULL;
	}

	if (!_display_init (d, width, height, scaled_w, scaled_h)) {
		fprintf (stderr, "%s: _display_init failed\n", __func__);
		free (d);
		d = NULL;
	} else {
		printf ("%s: display width=%" PRIu32 ", height=%" PRIu32 ", "
		        "scaled_w=%" PRIu32 ", scaled_h=%" PRIu32 "\n",
		        __func__, d->width, d->height, d->scaled_w, d->scaled_h);
	}

	return d;
}

void
display_destroy (struct display *d)
{
	if (d) {
		_display_fini (d);
		free (d);
		d = NULL;
	}
}

uint32_t
display_width (struct display *d)
{
	return (d) ? d->width : 0;
}

uint32_t
display_height (struct display *d)
{
	return (d) ? d->height : 0;
}

uint32_t
display_scaled_width (struct display *d)
{
	return (d) ? d->scaled_w : 0;
}

uint32_t
display_scaled_height (struct display *d)
{
	return (d) ? d->scaled_h : 0;
}

uint32_t *
display_pixbuf (struct display *d)
{
	return (d) ? d->pixbuf : NULL;
}


__attribute__((always_inline))
static inline void
_display_render_bg (uint32_t *dst,
                    uint32_t  w,
                    uint32_t  h,
                    uint32_t *bg_data)
{
	size_t n = w * h;
	uint32_t *d = dst, *b = bg_data;
	for (size_t i = 0; i < n; ++i) {
		*d++ = *b++;
	}
}

bool
display_render_bg (struct display *d,
                   uint32_t       *bg_data)
{
	if (d && bg_data) {
		_display_render_bg (d->pixbuf, d->width, d->height, bg_data);
		return true;
	}
	fprintf (stderr, "%s: invalid arguments\n", __func__);
	return false;
}

typedef union {
	uint32_t u32;
	struct {
		uint8_t b: 8;
		uint8_t g: 8;
		uint8_t r: 8;
		uint8_t a: 8;
	} __attribute__((gcc_struct,packed));
} argb_t;

typedef union __attribute__((transparent_union)) {
	int32_t  i;
	uint32_t u;
} val32_t;

__attribute__((always_inline))
static inline bool
_dim_params (uint32_t  s, //! Dimension size
             uint32_t  b, //! Bounding dimension size
             val32_t   o, //! Offset relative to beginning of bounding dimension
             uint32_t *r, //! Resulting size
             uint32_t *i, //! Resulting internal offset
             uint32_t *e) //! Resulting external offset
{
	val32_t v;
	uint32_t u;

	if (o.i < 0) {
		// Dimension begins before bounding dimension
		v.i = (o.i == INT32_MIN) ? o.i : -o.i;

		if (v.u >= s) {
			return false;
		}

		u = s - v.u;
		*r = (u > b) ? b : u;
		*i = v.u;
		*e = 0;

	} else if (o.u < b) {
		// Beginning of dimension is located within bounding dimension

		u = b - o.u;
		*r = (s > u) ? u : s;
		*i = 0;
		*e = o.u;

	} else {
		return false;
	}

	return true;
}

__attribute__((always_inline))
static inline void
_display_render (uint32_t *dst,
                 uint32_t  w,
                 uint32_t  h,
                 uint32_t *bg_data,
                 uint32_t *ol_data,
                 uint32_t  ol_w,
                 uint32_t  ol_h,
                 int32_t   ol_x,
                 int32_t   ol_y)
{
	uint32_t w0, w1, w2, h0, h1, h2, rect_x, rect_y, *dp, *bp, *op;
	unsigned int y, x;

	if (!_dim_params (ol_w, w, ol_x, &w1, &rect_x, &w0)
	    || !_dim_params (ol_h, h, ol_y, &h1, &rect_y, &h0)) {
		_display_render_bg (dst, w, h, bg_data);
		return;
	}

	w2 = w - (w0 + w1);
	h2 = h - (h0 + h1);
	dp = dst;
	bp = bg_data;
	op = &ol_data[(rect_y * ol_w) + rect_x];

	// vertical section above overlay
	for (y = 0; y < h0; ++y) {
		for (x = 0; x < w; ++x) {
			*dp++ = *bp++;
		}
	}

	// vertical section containing overlay
	for (y = 0; y < h1; ++y) {
		// horizontal section at left side of overlay
		for (x = 0; x < w0; ++x) {
			*dp++ = *bp++;
		}

		// overlay
		for (x = 0; x < w1; ++x) {
			// A over B premultiplied pixel sum
			argb_t a = { .u32 = op[x] },
			       b = { .u32 = *bp++ };
			float f = 1.0f - (a.a / 255.0f);
			argb_t c = {
				.a = 255,
				.r = a.r + (uint8_t) (b.r * f),
				.g = a.g + (uint8_t) (b.g * f),
				.b = a.b + (uint8_t) (b.b * f)
			};
			*dp++ = c.u32;
		}

		// horizontal section at right side of overlay
		for (x = 0; x < w2; ++x) {
			*dp++ = *bp++;
		}

		op += ol_w;
	}

	// vertical section below overlay
	for (y = 0; y < h2; ++y) {
		for (x = 0; x < w; ++x) {
			*dp++ = *bp++;
		}
	}
}

bool
display_render (struct display *d,
                uint32_t       *bg_data,
                uint32_t       *ol_data,
                uint32_t        ol_w,
                uint32_t        ol_h,
                int32_t         ol_x,
                int32_t         ol_y)
{
	if (d && bg_data && ol_data) {
		_display_render (d->pixbuf, d->width, d->height, bg_data,
		                 ol_data, ol_w, ol_h, ol_x, ol_y);
		return true;
	}
	fprintf (stderr, "%s: invalid arguments\n", __func__);
	return false;
}

struct display_modpkt *
display_modpkt_inspect (const char *buf,
                        int         len)
{
	size_t n;
	struct display_modpkt *p;

	n = len;

	if (n < offsetof (struct display_modpkt, name)) {
		goto _packet_too_short;
	}

	n -= offsetof (struct display_modpkt, name);
	p = (struct display_modpkt *) buf;

	if (n < p->nsiz) {
	_packet_too_short:
		fprintf (stderr, "%s: packet too short\n", __func__);
		return NULL;
	}

	if (p->dims.w < 1 || p->dims.h < 1) {
		fprintf (stderr,
		         "%s: packet defines invalid width and/or height\n",
		         __func__);
		return NULL;
	}

	return p;

}

bool
display_banner_uuid_export (char   *path,
                            size_t  path_len)
{
	struct banner *b;
	bool r;
	file_t *f;
	unsigned int k, l, i;
	union {
		char     c[40];
		uint64_t u64[5];
	} buf = {.u64 = {0, 0, 0, 0, 0}};
	union {
		char          *chr;
		const char    *cch;
		const uint8_t *cu8;
	} ptr;

	if (!(b = cur_banner)) {
		return false;
	}

	path[path_len] = '0';
	path[path_len + 1] = '\0';

	if (!(f = file_create (path))) {
		fprintf (stderr, "%s: file_create failed\n", __func__);
		r = false;
		goto _end;
	}

	for (k = 0, l = 0; k < 16; ++k) {
		i = b->uuid[k];
		buf.c[l++] = _hex_char (i >> 4);
		buf.c[l++] = _hex_char (i & 0x0f);
	}
	buf.c[l] = '\0';

	if (!file_open (f, "w")) {
		ptr.cch = "open";
		goto _fail;
	}

	ptr.chr = buf.c;

	if (!file_write (f, 32, ptr.cu8)) {
		(void) file_close (f);
		ptr.cch = "write";
		goto _fail;
	}

	if (!file_close (f)) {
		ptr.cch = "close";
	_fail:
		fprintf (stderr, "%s: file_%s failed\n", __func__, ptr.cch);
		r = false;
		goto _destroy_file;
	}

	r = true;

_destroy_file:
	file_destroy (&f);

	buf.u64[0] = 0;
	buf.u64[1] = 0;
	buf.u64[2] = 0;
	buf.u64[3] = 0;
	buf.u64[4] = 0;
	ptr.chr = NULL;

_end:
	path[path_len] = '\0';

	return r;
}
