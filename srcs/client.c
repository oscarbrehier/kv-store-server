#include "common.h"
#include "commands.h"
#include "kv_table.h"

void    handle_client_input(int socket, char *input)
{   
    int     argc;
    char    **argv;

    parse_input(input, &argc, &argv);
    if (!argc)
    {
		return ;
	}
	command_exec(socket, argc, argv);
	free_argv(argv);
}

void	client_send(int fd, char *format, ...)
{
	va_list	args;
	va_list	args_copy;
	char	*formatted;
	char	*message;
	int		len;

	if (!format)
		return ;
	va_start(args, format);
	va_copy(args_copy, args);
	len = vsnprintf(NULL, 0, format, args_copy);
	va_end(args_copy);
	if (len < 0)
	{
		va_end(args);
		return ;
	}
	formatted = malloc(sizeof(char) * (len + 1));
	if (!formatted)
	{
		va_end(args);
		return ;
	}
	vsnprintf(formatted, len + 1, format, args);
	va_end(args);
	message = malloc(sizeof(char) * (len + 2));
	if (!message)
	{
		free(formatted);
		return ;
	}
	memcpy(message, formatted, len);
	message[len] = '\n';
	message[len + 1] = '\0';
	send(fd, message, len + 1, 0);
	free(formatted);
	free(message);
}