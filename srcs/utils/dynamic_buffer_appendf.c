#include "common.h"
#include "utils/dynamic_buffer.h"

int	resize_content(char **content, int *content_capacity, int add_size)
{
	char	*temp;
	
	*content_capacity = (*content_capacity * 2 ) + add_size;
	temp = realloc(*content, *content_capacity + 1);
	if (!temp)
	{	
		free(*content);
		*content = NULL;
		return (-1);
	}
	*content = temp;
	return (0);
}

void	dynamic_buffer_appendf(t_dynamic_buffer **buffer, const char *format, ...)
{
	int		i;
	va_list	args;
	const char	*arg;
	char	*content;
	int		content_len;
	int		content_capacity;

	va_start(args, format);
	content_len = 0;
	content_capacity = strlen(format);
	content = (char *)malloc(sizeof(char) * (content_capacity + 1));
	if (!content)
	{
		dynamic_buffer_append(*buffer, format, strlen(format));
		return ;
	}
	i = 0;
	while (format[i])
	{
		if (format[i] == '%')
		{
			i++;
			if (format[i] == '\0') {
                content[content_len++] = '%';
                break;
            }
			switch (format[i])
			{
				case 's': {
					arg = va_arg(args, const char *);
					if (arg == NULL)
					{
						arg = "(null)";
					}
					int arg_len = strlen(arg);
					if (content_len + arg_len >= content_capacity)
					{
						if (resize_content(&content, &content_capacity, arg_len) != 0)
						{
							dynamic_buffer_append(*buffer, format, strlen(format));
							va_end(args);
							return ; 
						}
					}
					memcpy(content + content_len, arg, arg_len);
					content_len += arg_len;
					break ;
				}
				
				default: {
					if (content_len + 2 >= content_capacity)
					{
						if (resize_content(&content, &content_capacity, 2) != 0)
						{
							dynamic_buffer_append(*buffer, format, strlen(format));
							va_end(args);
							return ;
						}
					}
					content[content_len++] = '%';
					content[content_len++] = format[i];
                    break ;
				}
			}
		}
		else
		{
			if (content_len + 2 >= content_capacity)
			{
				if (resize_content(&content, &content_capacity, 1) != 0)
				{
					dynamic_buffer_append(*buffer, format, strlen(format));
					va_end(args);
					return ;
				}
			}
			content[content_len++] = format[i];
		}
		i++;
	}
	va_end(args);
	content[content_len] = '\0';
	dynamic_buffer_append(*buffer, content, content_len);
	free(content);
}
