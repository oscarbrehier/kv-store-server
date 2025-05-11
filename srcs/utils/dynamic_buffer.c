#include "utils/dynamic_buffer.h"

t_dynamic_buffer	*dynamic_buffer_create(size_t size)
{
	t_dynamic_buffer	*buffer;

	buffer = malloc(sizeof(t_dynamic_buffer));
	if (!buffer)
		return (NULL);
	buffer->buffer = malloc(sizeof(char) * (size + 1));
	if (!buffer->buffer)
	{
		free(buffer);
		return (NULL);
	}
	buffer->size = size;
	buffer->used = 0;
	return (buffer);
}

void	dynamic_buffer_destroy(t_dynamic_buffer *buffer)
{
	if (buffer)
	{
		free(buffer->buffer);
		free(buffer);
	}
}

void	dynamic_buffer_append(t_dynamic_buffer *buffer, const char *data, size_t length)
{
	size_t	new_size;
	char	*new_buffer;

	if (!buffer || !data)
		return ;
	if (buffer->used + length >= buffer->size)
	{
		new_size = buffer->size * 2;
		if (new_size < buffer->used + length)
		{
			new_size = buffer->used + length + 1024;
		}
		new_buffer = realloc(buffer->buffer, new_size);
		if (!new_buffer)
			return ;
		buffer->buffer = new_buffer;
		buffer->size = new_size;
	}
	memcpy(buffer->buffer + buffer->used, data, length);
	buffer->used += length;
	buffer->buffer[buffer->used] = '\0';
}

void	dynamic_buffer_reset(t_dynamic_buffer *buffer)
{
	if (buffer)
	{
		buffer->used = 0;
		if (buffer->size > 0)
			buffer->buffer[0] = '\0';
	}
}