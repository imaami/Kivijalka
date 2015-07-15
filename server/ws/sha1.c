#include "private/sha1.h"

void
sha1_init (sha1_t *hash)
{
	if (hash) {
		_sha1_init (hash);
	}
}

void
sha1_gen (sha1_t  *hash,
          size_t   size,
          uint8_t *data)
{
	if (hash && data) {
		_sha1_gen (hash, size, data);
	}
}

void
sha1_str (sha1_t *hash,
          char   *dest)
{
	if (hash && dest) {
		_sha1_str (hash, dest);
	}
}
