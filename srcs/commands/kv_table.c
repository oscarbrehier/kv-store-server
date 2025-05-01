#include "kv_table.h"
#include "commands.h"
#include "client.h"
#include "g_table.h"

void	handle_get(int socket, int argc, char **argv)
{
	(void)argc;
	t_status_code   status;
    char            *output;

	status = kv_get(g_table, argv[1], (void *)&output, STRING);
    if (status == SUCCESS_CODE)
	{
        client_send(socket, output);
        return ;
    }
    client_send(socket, "null");
}

DEFINE_COMMAND(handle_get, get, "get <key>", "", 1);