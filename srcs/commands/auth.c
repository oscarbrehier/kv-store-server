#include "globals.h"
#include "auth.h"
#include "utils/dynamic_buffer.h"
#include "status_codes.h"
#include "commands.h"
#include "status_codes.h"

t_status	handle_register(t_dynamic_buffer **buffer, int argc, char **argv)
{
	(void)argc;
	t_status	status;

	status = auth_register(argv[1], argv[2]);
	if (status.code != SUCCESS)
	{
		dynamic_buffer_appendf(buffer, "%s", status_messages[status.code]);
		return (status);
	}
	dynamic_buffer_append(*buffer, "OK\n", strlen("OK\n"));
	return (status);
}

DEFINE_COMMAND(handle_register, register, "register <username> <password>", "", 2, T_READ);