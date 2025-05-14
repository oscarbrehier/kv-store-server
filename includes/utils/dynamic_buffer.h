#ifndef DYNAMIC_BUFFER_H
# define DYNAMIC_BUFFER_H

#include "common.h"

typedef struct s_dynamic_buffer
{
	char	*buffer;
	size_t	size;
	size_t	used;
} t_dynamic_buffer;

t_dynamic_buffer	*dynamic_buffer_create(size_t size);
void				dynamic_buffer_append(t_dynamic_buffer *buffer, const char *data, size_t length);
void				dynamic_buffer_appendf(t_dynamic_buffer **buffer, char *data, ...);
void				dynamic_buffer_destroy(t_dynamic_buffer *buffer);
void				dynamic_buffer_reset(t_dynamic_buffer *buffer);

#endif