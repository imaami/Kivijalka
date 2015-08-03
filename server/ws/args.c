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
args_get_banner_cache_path (void)
{
	return _args_get_banner_cache_path (&args);
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
