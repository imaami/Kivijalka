#include "private/hex.h"

const char const HEX_CHAR[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
};

char
hex_char (unsigned int i)
{
	return _hex_char (i & 0x0f);
}
