#include "kv_table.h"
#include "commands.h"
#include "client.h"
#include "g_table.h"

int handle_get(int socket, int argc, char **argv)
{
	(void)argc;
	t_status_code   status;
    char            *output;

	status = kv_get(g_table, argv[1], (void *)&output, STRING);
    if (status == SUCCESS_CODE)
	{
        client_send(socket, output);
        return (0);
    }
    client_send(socket, "null");
    return (-1);
}

int handle_set(int socket, int argc, char **argv)
{
    (void)argc;
    t_status_code   status;

    status = kv_set(g_table, argv[1], (void *)argv[2], strlen(argv[2]), STRING);
    if (status == SUCCESS_CODE)
    {
        client_send(socket, "OK");
        return (0);
    }
    client_send(socket, "null");
    return (-1);
}

DEFINE_COMMAND(handle_get, get, "get <key>", "", 1, T_READ);
DEFINE_COMMAND(handle_set, set, "set <key> <value>", "", 2, T_WRITE);