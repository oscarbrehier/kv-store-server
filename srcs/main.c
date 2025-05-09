#include "common.h"
#include "globals.h"
#include "server.h"
#include "kv_table.h"

volatile int running = 1;
t_server_config	*config = NULL;

int	main(void)
{
	config = server_config_create(DEFAULT_PORT, DEFAULT_BACKLOG, DEFAULT_MAX_CLIENTS, DEFAULT_THREAD_POOL_SIZE);
	if (!config)
	{
		fprintf(stderr, "Failed to create server configuration\n");
		return (1);
	}
	if (server_start(config) != 0)
	{
		fprintf(stderr, "Server failed to start\n");
		server_config_destroy(config);
		return (1);
	}
	server_config_destroy(config);
	return (1);
}