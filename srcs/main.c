#include "common.h"
#include "globals.h"
#include "server.h"
#include "kv_table.h"
#include "g_table.h"

volatile int running = 1;
t_server_config	*config = NULL;

int	main(void)
{
	pthread_t	autosave_thread;

	config = server_config_create(DEFAULT_PORT, DEFAULT_BACKLOG, DEFAULT_MAX_CLIENTS, DEFAULT_THREAD_POOL_SIZE);
	if (!config)
	{
		fprintf(stderr, "Failed to create server configuration\n");
		return (1);
	}
	if (init_global_table() != 0)
	{
		fprintf(stderr, "Failed to initialize table\n");
		return (1);
	}
	if (pthread_create(&autosave_thread, NULL, g_table_autosave, NULL) != 0)
	{
		fprintf(stderr, "Failed to start autosave thread\n");
		return (1);
	}
	if (server_start(config) != 0)
	{
		fprintf(stderr, "Server failed to start\n");
		server_config_destroy(config);
		return (1);
	}
	pthread_join(autosave_thread, NULL);
	if (config)
	{
		server_stop(config);
		server_config_destroy(config);
		if (g_table)
		{
			kv_free_table(g_table);
			g_table = NULL;
		}
	}
	return (0);
}