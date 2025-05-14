#include "common.h"
#include "utils/utils.h"
#include "utils/dynamic_buffer.h"

void	dynamic_buffer_appendf(t_dynamic_buffer **buffer, char *format, ...)
{
	va_list		args;
	char		*content;

	va_start(args, format);
	content = stringf(format, args);
	if (content == NULL)
	{
		dynamic_buffer_append(*buffer, format, strlen(format));
		va_end(args);
		return ;
	}
	va_end(args);
	dynamic_buffer_append(*buffer, content, strlen(content));
	free(content);
}
