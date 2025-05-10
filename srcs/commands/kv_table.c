#include "kv_table.h"
#include "commands.h"
#include "client.h"
#include "g_table.h"
#include "utils/dyanmic_buffer.h"
#include "globals.h"

int handle_get(t_dynamic_buffer **buffer, int argc, char **argv)
{
	(void)argc;
	(void)socket;
	t_status_code   status;
    char            *output;

    if (!running)
    {
		return (-1);
	}
	status = kv_get(g_table, argv[1], (void *)&output, STRING);
    if (status == SUCCESS_CODE)
	{
        // client_send(socket, output);
        dynamic_buffer_append(*buffer, output, strlen(output));
        return (0);
    }
    dynamic_buffer_append(*buffer, "null", strlen("null"));
    return (-1);
}

int handle_set(t_dynamic_buffer **buffer, int argc, char **argv)
{
    (void)argc;
    (void)buffer;
    t_status_code   status;

    status = kv_set(g_table, argv[1], (void *)argv[2], strlen(argv[2]), STRING);
    if (status == SUCCESS_CODE)
    {
        // client_send(socket, "OK");
        return (0);
    }
    // client_send(socket, "null");
    return (-1);
}

DEFINE_COMMAND(handle_get, get, "get <key>", "", 1, T_READ);
DEFINE_COMMAND(handle_set, set, "set <key> <value>", "", 2, T_WRITE);