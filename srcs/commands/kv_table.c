#include "kv_table.h"
#include "commands.h"
#include "client.h"
#include "g_table.h"
#include "utils/dynamic_buffer.h"
#include "globals.h"
#include "status_codes.h"

t_status    handle_get(t_dynamic_buffer **buffer, int argc, char **argv, ...)
{
	(void)argc;
	t_status	status;
    char		*output;

	status = kv_get(g_table, argv[1], (void **)&output, STRING);
    switch (status.code)
    {
        case SUCCESS:
        {
            dynamic_buffer_appendf(buffer, "%s\n", output);
            break ;
        }
        case ERROR_KEY_NOT_FOUND:
        {
            dynamic_buffer_appendf(buffer, "key not found\n");
            break ;
        }
        default:
        {
            dynamic_buffer_appendf(buffer, "null\n");
            break ;
        }
    }
    return (status);
}

t_status handle_set(t_dynamic_buffer **buffer, int argc, char **argv, ...)
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
		.hide = 0,
        .min_len = 1,
        .max_len = 32
	}
};

static t_arg	set_args[] = {
	{
		.hide = 0,
        .min_len = 1,
        .max_len = 32
	},
    {
		.hide = 0,
        .min_len = 1,
        .max_len = 256
	}
};

DEFINE_COMMAND(handle_get, GET, "GET <key>", "", 1, T_READ | AUTH, get_args);
DEFINE_COMMAND(handle_set, SET, "SET <key> <value>", "", 2, T_WRITE | AUTH, set_args);