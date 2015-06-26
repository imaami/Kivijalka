/* read-file.c -- read file contents into a string
   Copyright (C) 2006, 2009-2015 Free Software Foundation, Inc.
   Written by Simon Josefsson and Bruno Haible.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#include "read-file.h"

/* Get fstat.  */
#include <sys/stat.h>

/* Get ftello.  */
#include <stdio.h>

/* Get SIZE_MAX.  */
#include <stdint.h>

/* Get malloc, realloc, free. */
#include <stdlib.h>

/* Get errno. */
#include <errno.h>

/* Read a STREAM and return a newly allocated string with the content,
   and set *LENGTH to the length of the string.  The string is
   zero-terminated, but the terminating zero byte is not counted in
   *LENGTH.  On errors, *LENGTH is undefined, errno preserves the
   values set by system functions (if any), and NULL is returned.  */
char *
fread_file (FILE *stream, size_t *length, size_t dest_off)
{
	char *buf = NULL;
	size_t alloc, alloc_total;

	/* For a regular file, allocate a buffer that has exactly the right
	   size.  This avoids the need to do dynamic reallocations later.  */
	struct stat st;

	if (fstat (fileno (stream), &st) >= 0 && S_ISREG (st.st_mode)) {
		off_t pos = ftello (stream);

		if (pos >= 0 && pos < st.st_size) {
			size_t alloc_off = st.st_size - pos;

			/* '1' below, accounts for the trailing NUL.  */
			if ((SIZE_MAX - dest_off) - 1 < alloc_off) {
				errno = ENOMEM;
				return NULL;
			}

			alloc = alloc_off + 1;
		} else {
			goto unknown_file_size;
		}
	} else {
	unknown_file_size:
		alloc = BUFSIZ;
	}

	alloc_total = alloc + dest_off;

	if (!(buf = malloc (alloc_total))) {
		return NULL; /* errno is ENOMEM.  */
	}

	size_t size = 0; /* number of bytes read so far */
	int save_errno;

	for (;;) {
		/* This reads 1 more than the size of a regular file
		   so that we get eof immediately.  */
		size_t requested = alloc - size;
		size_t count = fread (buf + dest_off + size, 1, requested, stream);
		size += count;

		if (count != requested) {
			save_errno = errno;
			if (ferror (stream)) {
				break;
			}

			/* Shrink the allocated memory if possible.  */
			if (size < alloc - 1) {
				char *smaller_buf;
				if ((smaller_buf = realloc (buf, dest_off + size + 1))) {
					buf = smaller_buf;
				}
			}

			buf[dest_off + size] = '\0';
			*length = size;
			return buf;
		}

		char *new_buf;

		if (alloc_total == SIZE_MAX) {
			save_errno = ENOMEM;
			break;
		}

		if (alloc_total < SIZE_MAX - alloc_total / 2) {
			alloc_total = alloc_total + alloc_total / 2;
		} else {
			alloc_total = SIZE_MAX;
		}

		alloc = alloc_total - dest_off;

		if (!(new_buf = realloc (buf, alloc_total))) {
			save_errno = errno;
			break;
		}

		buf = new_buf;
	}

	free (buf);
	errno = save_errno;
	return NULL;
}

static char *
internal_read_file (const char *filename, size_t *length, const char *mode, size_t dest_off)
{
	FILE *stream = fopen (filename, mode);
	char *out;
	int save_errno;

	if (!stream) {
		return NULL;
	}

	out = fread_file (stream, length, dest_off);

	save_errno = errno;

	if (fclose (stream) != 0) {
		if (out) {
			save_errno = errno;
			free (out);
		}
		errno = save_errno;
		return NULL;
	}

	return out;
}

/* Open and read the contents of FILENAME, and return a newly
   allocated string with the content, and set *LENGTH to the length of
   the string.  The string is zero-terminated, but the terminating
   zero byte is not counted in *LENGTH.  On errors, *LENGTH is
   undefined, errno preserves the values set by system functions (if
   any), and NULL is returned.  */
char *
read_file (const char *filename, size_t *length, size_t dest_off)
{
	return internal_read_file (filename, length, "r", dest_off);
}

/* Open (on non-POSIX systems, in binary mode) and read the contents
   of FILENAME, and return a newly allocated string with the content,
   and set LENGTH to the length of the string.  The string is
   zero-terminated, but the terminating zero byte is not counted in
   the LENGTH variable.  On errors, *LENGTH is undefined, errno
   preserves the values set by system functions (if any), and NULL is
   returned.  */
char *
read_binary_file (const char *filename, size_t *length, size_t dest_off)
{
	return internal_read_file (filename, length, "rb", dest_off);
}
