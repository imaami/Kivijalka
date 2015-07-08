/** \file buf.c
 *
 * Memory buffer object.
 */

#include "private/buf.h"

/**
 * @brief Create (allocate) a new data buffer.
 * @param size Number of bytes the buffer should have available.
 * @return Pointer to newly allocated buffer object if the operation
 *         succeeded, NULL otherwise.
 */
struct buf *
buf_create (size_t size)
{
	return _buf_create (size);
}

/**
 * @brief Destroy a buffer object.
 * @param b Pointer to the buffer pointer.
 */
void
buf_destroy (struct buf **b)
{
	if (b) {
		_buf_destroy (b);
	}
}

/**
 * @brief Allocate (reserve) space within an existing buffer object.
 * @param b Pointer to the buffer pointer.
 * @param size Number of bytes to allocate.
 * @return Pointer to the first byte of the allocated space
 *         if the operation succeeded, NULL otherwise. Do not
 *         free() the returned pointer - it is just a pointer
 *         into the buffer object's internal data array, not
 *         an allocated object.
 */
uint8_t *
buf_alloc (struct buf *b,
           size_t      size)
{
	return (b) ? _buf_alloc (b, size) : NULL;
}

/**
 * @brief Reset a buffer's used bytes counter. No data is zeroed out,
 *        but the next call to \a buf_alloc will return a pointer
 *        which points the beginning of the data array, and writing to
 *        it will therefore overwrite previously written bytes.
 * @param b Pointer to the buffer pointer.
 * @return Pointer to the first byte of the buffer's data array.
 */
char *
buf_reset (struct buf *b)
{
	return (b) ? _buf_reset (b) : NULL;
}
