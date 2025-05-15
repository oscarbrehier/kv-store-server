#include "kv_table.h"
#include "commands.h"
#include "client.h"
#include "g_table.h"
#include "utils/dynamic_buffer.h"
#include "globals.h"
#include "status_codes.h"

t_status    handle_get(t_dynamic_buffer **buffer, int argc, char **argv)
{
	(void)argc;
	(void)socket;
	t_status	status;
    char		*output;

	status = kv_get(g_table, argv[1], (void *)&output, STRING);
    if (status.code == SUCCESS)
	{
        dynamic_buffer_appendf(buffer, "%s\n", output);
        return (status);
    }
    dynamic_buffer_appendf(buffer, "null\n");
    return (status);
}

t_status handle_set(t_dynamic_buffer **buffer, int argc, char **argv)
{
    (void)argc;
    (void)buffer;
    t_status	status;

    status = kv_set(g_table, argv[1], (void *)argv[2], strlen(argv[2]), STRING);
    if (status.code == SUCCESS)
    {
        dynamic_buffer_appendf(buffer, "OK\n");
        return (status);
    }
    dynamic_buffer_appendf(buffer, "null\n");
    return (status);
}

static t_arg	get_args[] = {
	{
		.hide = 0
	}
};

static t_arg	set_args[] = {
	{
		.hide = 0
	},
    {
		.hide = 0
	}
};

DEFINE_COMMAND(handle_get, get, "get <key>", "", 1, T_READ, get_args);
DEFINE_COMMAND(handle_set, set, "set <key> <value>", "", 2, T_WRITE, set_args);