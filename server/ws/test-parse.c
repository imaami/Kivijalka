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
#include "private/parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <uuid/uuid.h>

int
main (int    argc,
      char **argv)
{
	if (argc < 2 || !argv) {
		return EXIT_FAILURE;
	}

	puts ("arg  pos  type  value");

	unsigned int i = 1;
	while (i < argc) {
		char *ptr = argv[i];
		unsigned int pos = 0;
		while (ptr[pos]) {
			if (ptr[pos] == 'u') {
				if (ptr[pos + 1] == '8'
				    && ptr[pos + 2] == ':') {
					pos += 3;
					uint8_t v;
					printf ("%3.u  %3.d   u8   ", i, pos);
					if (!_parse_u8 (ptr, &pos, &v)) {
						printf ("%s\n",
							(v == UINT8_MAX)
							? "overflow"
							: "error");
						goto _next_arg;
					}
					printf ("%" PRIu8 "\n", v);

				} else if (ptr[pos + 1] == '1'
				           && ptr[pos + 2] == '6'
				           && ptr[pos + 3] == ':') {
					pos += 4;
					uint16_t v;
					printf ("%3.u  %3.d  u16   ", i, pos);
					if (!_parse_u16 (ptr, &pos, &v)) {
						printf ("%s\n",
							(v == UINT16_MAX)
							? "overflow"
							: "error");
						goto _next_arg;
					}
					printf ("%" PRIu16 "\n", v);

				} else if (ptr[pos + 1] == '3'
				           && ptr[pos + 2] == '2'
				           && ptr[pos + 3] == ':') {
					pos += 4;
					uint32_t v;
					printf ("%3.u  %3.d  u32   ", i, pos);
					if (!_parse_u32 (ptr, &pos, &v)) {
						printf ("%s\n",
							(v == UINT32_MAX)
							? "overflow"
							: "error");
						goto _next_arg;
					}
					printf ("%" PRIu32 "\n", v);

				} else if (ptr[pos + 1] == '6'
				           && ptr[pos + 2] == '4'
				           && ptr[pos + 3] == ':') {
					pos += 4;
					uint64_t v;
					printf ("%3.u  %3.d  u64   ", i, pos);
					if (!_parse_u64 (ptr, &pos, &v)) {
						printf ("%s\n",
							(v == UINT64_MAX)
							? "overflow"
							: "error");
						goto _next_arg;
					}
					printf ("%" PRIu64 "\n", v);

				} else {
					++pos;
				}

			} else if (ptr[pos] == 'i') {
				if (ptr[pos + 1] == '1'
				    && ptr[pos + 2] == '6'
				    && ptr[pos + 3] == ':') {
					pos += 4;
					int16_t v;
					printf ("%3.u  %3.d  i16   ", i, pos);
					if (!_parse_i16 (ptr, &pos, &v)) {
						printf ("%s\n",
						        (v == INT16_MAX)
						        ? "overflow"
						        : (v == INT16_MIN)
						          ? "underflow"
						          : "error");
						goto _next_arg;
					}
					printf ("%" PRId16 "\n", v);

				} else if (ptr[pos + 1] == '3'
				           && ptr[pos + 2] == '2'
				           && ptr[pos + 3] == ':') {
					pos += 4;
					int32_t v;
					printf ("%3.u  %3.d  i32   ", i, pos);
					if (!_parse_i32 (ptr, &pos, &v)) {
						printf ("%s\n",
						        (v == INT32_MAX)
						        ? "overflow"
						        : (v == INT32_MIN)
						          ? "underflow"
						          : "error");
						goto _next_arg;
					}
					printf ("%" PRId32 "\n", v);

				} else if (ptr[pos + 1] == '6'
				           && ptr[pos + 2] == '4'
				           && ptr[pos + 3] == ':') {
					pos += 4;
					int64_t v;
					printf ("%3.u  %3.d  i64   ", i, pos);
					if (!_parse_i64 (ptr, &pos, &v)) {
						printf ("%s\n",
						        (v == INT64_MAX)
						        ? "overflow"
						        : (v == INT64_MIN)
						          ? "underflow"
						          : "error");
						goto _next_arg;
					}
					printf ("%" PRId64 "\n", v);

				} else {
					++pos;
				}

			} else if (ptr[pos] == 'g') {
				if (ptr[pos + 1] == ':') {
					pos += 2;
					uint32_t w, h;
					int32_t x, y;
					printf ("%3.u  %3.d  geo   ", i, pos);
					if (!_parse_xgeo (ptr, &pos,
					                  &w, &h, &x, &y)) {
						puts ("error");
						goto _next_arg;
					}
					printf ("%" PRIu32 "x%" PRIu32
					        "%s%" PRId32 "%s%" PRId32 "\n",
					        w, h,
					        (x >= 0) ? "+" : "", x,
					        (y >= 0) ? "+" : "", y);

				} else {
					++pos;
				}

			} else if (ptr[pos] == 'U') {
				if (ptr[pos + 1] == ':') {
					pos += 2;
					printf ("%3.u  %3.d  uuid  ", i, pos);
					uuid_t uuid;
					size_t n;
					if (!_parse_uuid (ptr + pos, uuid, &n)) {
						printf ("error at %zu\n", n);
						goto _next_arg;
					}
					printf ("\n");

				} else {
					++pos;
				}

			} else {
				++pos;
			}
		}

	_next_arg:
		++i;
	}

	return EXIT_SUCCESS;
}
