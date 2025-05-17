#include "globals.h"
#include "common.h"	
#include "commands.h"
#include "utils/dynamic_buffer.h"
#include "client.h"

int	validate_flags(int flags, t_client client, t_dynamic_buffer **buffer)
{
	if ((flags & AUTH) && client.authenticated != 1)
	{
		dynamic_buffer_appendf(buffer, "authenticated required to run this command\n");
		return (-1);
	}
	if ((flags & NO_AUTH) && client.authenticated == 1)
	{
		dynamic_buffer_appendf(buffer, "client already authenticated\n");
		return (-1);
	}
	return (0);
}

int	validate_usage(t_dynamic_buffer **buffer, t_command *command, int argc)
{
	if ((argc - 1) != command->arg_count)
	{
		dynamic_buffer_appendf(buffer, "(usage): %s\n", command->usage);
		return (FN_ERROR);
	}
	return (FN_SUCCESS);
}

int validate_args(t_dynamic_buffer **buffer, t_command *command, char **argv)
{
	int		i;
	int		arg_len;
	t_arg	arg;

	i = 1;
	arg_len = -1;
	while (argv[i])
	{
		arg_len = strlen(argv[i]);
		arg = command->args[i - 1];
		if (arg_len < arg.min_len)
		{
			dynamic_buffer_appendf(buffer, "argument %d is shorter than the minimum required length of %d characters\n", i, arg.min_len);
			return (-1);
		}
		else if (arg_len > arg.max_len)
		{
			dynamic_buffer_appendf(buffer, "argument %d exceeds maximum allowed length of %d characters\n", i, arg.max_len);
			return (-1);
		}
		i++;
	}
	return (0);
}

int validate_input(t_dynamic_buffer **buffer, t_command *command, t_client *client, int argc, char **argv)
{
	if (validate_flags(command->flags, *client, buffer) == FN_ERROR ||
		validate_usage(buffer, command, argc) ||
		validate_args(buffer, command, argv) == FN_ERROR
	)
	{
		return (FN_ERROR);
	}
	return (FN_SUCCESS);
}