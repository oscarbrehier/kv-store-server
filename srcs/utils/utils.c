#include "common.h"

int	numlen(long int nbr)
{
	int	len;

	len = 1;
	while (nbr / 10)
	{
		len++;
		nbr /= 10;
	}
	return (len);
}

char    *get_current_timestamp(time_t now)
{
    char 		*timestamp;
	struct tm	*tm_info;

	timestamp = (char *)malloc(sizeof(char) * 64);
	if (!timestamp)
	{
		perror("malloc failed");
		return (NULL);
	}
	tm_info = localtime(&now);
	strftime(timestamp, 64, "%Y-%m-%d %H:%M:%S", tm_info);
    return (timestamp);
}

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

char	*stringf(char *format, va_list args)
{
	int			i;
	int 		arg_len; 
	va_list		args_copy;
	const char	*arg;
	char		*content;
	int			content_len;
	int			content_capacity;

	va_copy(args_copy, args);
	content_len = 0;
	content_capacity = strlen(format);
	content = (char *)malloc(sizeof(char) * (content_capacity + 1));
	if (!content)
	{
		return (NULL);
	}
	i = 0;
	while (format[i])
	{
		arg_len = 0;
		if (format[i] == '%')
		{
			i++;
			if (format[i] == '\0') {
                content[content_len++] = '%';
                break ;
            }
			switch (format[i])
			{
				case 's':
				{
					arg = va_arg(args_copy, const char *);
					if (arg == NULL)
					{
						arg = "(null)";
					}
					arg_len = strlen(arg);
					if (content_len + arg_len >= content_capacity)
					{
						if (resize_content(&content, &content_capacity, arg_len) != 0)
						{
							va_end(args_copy);
							return (NULL); 
						}
					}
					memcpy(content + content_len, arg, arg_len);
					content_len += arg_len;
					break ;
				}
				case 'd':
				{
					int		num = va_arg(args_copy, int);
					char	*num_str;

					arg_len = snprintf(NULL, 0, "%d", num);
					num_str = (char *)malloc(sizeof(char) * (arg_len + 1));
					if (!num_str)
					{
						return (NULL);
					}
					snprintf(num_str, arg_len + 1, "%d", num);
					if (content_len + arg_len >= content_capacity && resize_content(&content, &content_capacity, arg_len) != 0)
					{
						va_end(args_copy);
						free(num_str);
						return (NULL); 
					}
					memcpy(content + content_len, num_str, arg_len);
					content_len += arg_len;
					free(num_str);
					break ;
				}
				default: 
				{
					if (content_len + 2 >= content_capacity)
					{
						if (resize_content(&content, &content_capacity, 2) != 0)
						{
							va_end(args_copy);
							return (NULL);
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
					va_end(args_copy);
					return (NULL);
				}
			}
			content[content_len++] = format[i];
		}
		i++;
	}
	va_end(args_copy);
	content[content_len] = '\0';
	return (content);
}