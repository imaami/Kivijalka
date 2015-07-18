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

bool
sha1_cmp (sha1_t *h1,
          sha1_t *h2)
{
	return (h1 && h2) ? _sha1_cmp (h1, h2) : false;
}

void
sha1_cpy (sha1_t *src,
          sha1_t *dest)
{
	if (src && dest) {
		_sha1_cpy (src, dest);
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
