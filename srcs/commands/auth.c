#include "globals.h"
#include "auth.h"
#include "utils/dynamic_buffer.h"
#include "status_codes.h"
#include "commands.h"
#include "status_codes.h"

t_status	handle_user_registration(t_dynamic_buffer **buffer, int argc, char **argv)
{
	(void)argc;
	t_status	status;

	status = auth_register_user(argv[1], argv[2]);
	if (status.code != SUCCESS)
	{
		dynamic_buffer_appendf(buffer, "%s\n", status_messages[status.code]);
		return (status);
	}
	dynamic_buffer_append(*buffer, "OK\n", strlen("OK\n"));
	return (status);
}

t_status	handle_user_login(t_dynamic_buffer **buffer, int argc, char **argv)
{
	(void)argc;
	t_status	status;

	status = auth_login_user(argv[1], argv[2]);
	if (status.code != SUCCESS)
	{
		dynamic_buffer_appendf(buffer, "%s\n", status_messages[status.code]);
		return (status);
	}
	dynamic_buffer_append(*buffer, "OK\n", strlen("OK\n"));
	return (status);
}

static t_arg	credentials_args[] = {
	{
		.hide = 0
	},
	{
		.hide = 1
	}
};

DEFINE_COMMAND(handle_user_registration, register, "register <username> <password>", "", 2, T_READ, credentials_args);
DEFINE_COMMAND(handle_user_login, login, "login <username> <password>", "", 2, T_READ, credentials_args);