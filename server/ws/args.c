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
/** \file args.c
 *
 * Command line args parsing.
 */

#include "private/args.h"

#include <stdint.h>
#include <stdbool.h>

static struct args args = ARGS_INIT;

bool
args_parse (int    argc,
            char **argv)
{
	if (argc < 1) {
		// nothing to parse
		return true;
	} else if (!argv || !argv[0]) {
		// natural order of things has been violated, the gods are angry
		return false;
	}

	return _args_parse (&args, argc, argv);
}

uint32_t
args_get_display_width (void)
{
	return _args_get_display_width (&args);
}

uint32_t
args_get_display_height (void)
{
	return _args_get_display_height (&args);
}

const char *
args_get_cache_path (void)
{
	return _args_get_cache_path (&args);
}

const char *
args_get_cache_uuid (void)
{
	return _args_get_cache_uuid (&args);
}

const char *
args_get_capture_path (void)
{
	return _args_get_capture_path (&args);
}

uint32_t
args_get_capture_width (void)
{
	return _args_get_capture_width (&args);
}

uint32_t
args_get_capture_height (void)
{
	return _args_get_capture_height (&args);
}

const char *
args_get_output_path (void)
{
	return _args_get_output_path (&args);
}

const char *
args_get_server_addr (void)
{
	return _args_get_server_addr (&args);
}

uint16_t
args_get_server_port (void)
{
	return _args_get_server_port (&args);
}
