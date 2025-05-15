#include "common.h"
#include "commands.h"
#include "client.h"
#include "logs.h"
#include "utils/utils.h"
#include "status_codes.h"

char	*format_argv(t_command cmd, int argc, char **argv)
{
	(void)cmd;
	int 	i;
	int		content_len;
	size_t	arg_len;
	char	*content;
	char	*ptr;

	i = 0;
	content_len = 0;
	while (i < argc)
	{
		if (i == 0)
			content_len += strlen(argv[0]);
		else
		{
			content_len += cmd.args[i - 1].hide ? 4 : strlen(argv[i]);
		}
		// content_len += cmd.args[i].hide ? 4 : strlen(argv[i]);
		// content_len += strlen(argv[i]);
		i++;
	}
	content_len += argc - 1;
	content_len += 1;
	content = (char *)malloc(sizeof(char) * (content_len + 1));
	if (!content)
	{
		perror("malloc failed");
		return (NULL);
	}
	i = 0;
	ptr = content;
	while (i < argc)
	{
		if (i == 0)
		{
			arg_len = strlen(argv[i]);
			memcpy(ptr, argv[i], arg_len);
		}
		else if (cmd.args[i - 1].hide)
		{
			arg_len = 4;
			memcpy(ptr, "****", arg_len);
		}
		else
		{
			arg_len = strlen(argv[i]);
			memcpy(ptr, argv[i], arg_len);
		}
		// memcpy(ptr, argv[i], arg_len);
		ptr += arg_len;
		if (i < argc - 1)
		{
			*ptr = ' ';
			ptr++;
		}
		i++;
	}
	*ptr = '\0';
	return (content);
}

void    command_logger(t_client client, t_command cmd, int argc, char **argv, t_status status)
{
    int		buffer_len;
	int		written;
    char	*buffer;
	char	*arguments;

	arguments = format_argv(cmd, argc, argv);
	buffer_len = snprintf(NULL, 0, "%s -> %s", arguments, status_messages[status.code]);
	if (buffer_len < 0)
	{
		perror("snprintf length calculation failed");
		free(arguments);
		return ;
	}
	buffer = (char *)malloc(sizeof(char) * (buffer_len + 1));
	if (!buffer)
	{
		fprintf(stderr, "Failed to log command\n");
		free(arguments);
		return ;
	}
	written = snprintf(buffer, buffer_len + 1, "%s -> %s", arguments, status_messages[status.code]);
	if (written < 0 || written > buffer_len)
	{
		perror("snprintf formatting failed");
		free(buffer);
		free(arguments);
		return ;
	}
	alog(status.log_level, client.ip, client.port, buffer);
	free(buffer);
	free(arguments);
}