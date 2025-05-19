#include "globals.h"
#include "common.h"
#include "auth.h"
#include "utils/dynamic_buffer.h"
#include "status_codes.h"
#include "commands.h"
#include "status_codes.h"
#include "client.h"
#include "logs.h"

t_status	handle_user_registration(t_dynamic_buffer **buffer, int argc, char **argv, ...)
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

t_status	handle_user_login(t_dynamic_buffer **buffer, int argc, char **argv, ...)
{
	(void)argc;
	t_status	status;
	t_client	*client;
	va_list		args;

	va_start(args, argv);
	client = va_arg(args, t_client *);
	va_end(args);
	if (!client)
	{
		dynamic_buffer_appendf(buffer, "%s\n", status_messages[AUTH_INTERNAL_ERROR]);
		return (status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR));
	}
	status = auth_login_user(argv[1], argv[2]);
	if (status.code != SUCCESS)
	{
		alogf(LOG_WARNING, client->ip, client->port, "Failed login for user %s", argv[1]);
		dynamic_buffer_appendf(buffer, "%s\n", status_messages[status.code]);
		return (status);
	}
	client->authenticated = 1;
	alogf(LOG_WARNING, client->ip, client->port, "Success login for user %s", argv[1]);
	dynamic_buffer_append(*buffer, "OK\n", strlen("OK\n"));
	return (status);
}

t_status	handle_user_logout(t_dynamic_buffer **buffer, int argc, char **argv, ...)
{
	(void)argc;
	(void)argv;
	t_client	*client;
	va_list		args;

	va_start(args, argv);
	client = va_arg(args, t_client *);
	va_end(args);
	if (!client)
	{
		dynamic_buffer_appendf(buffer, "%s\n", status_messages[AUTH_INTERNAL_ERROR]);
		return (status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR));
	}
	client->authenticated = 0;
	dynamic_buffer_append(*buffer, "OK\n", strlen("OK\n"));
	return (status_create(0, SUCCESS, LOG_INFO));
}

static t_arg	credentials_args[] = {
	{
		.hide = 0,
		.min_len = 3,
		.max_len = 64
	},
	{
		.hide = 1,
		.min_len = 3,
		.max_len = 64
	}
};

DEFINE_COMMAND(handle_user_registration, REGISTER, "REGISTER <username> <password>", "", 2, NO_AUTH, credentials_args);
DEFINE_COMMAND(handle_user_login, LOGIN, "LOGIN <username> <password>", "", 2, NO_AUTH, credentials_args);
DEFINE_COMMAND(handle_user_logout, LOGOUT, "LOGOUT", "", 0, AUTH, NULL);