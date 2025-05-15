#include "common.h"
#include "globals.h"
#include "server.h"
#include "kv_table.h"
#include "g_table.h"
#include "logs.h"
#include "auth.h"

volatile int	 running = 1;
int				g_log_file = -1;
t_server_config	*config = NULL;
pthread_mutex_t	log_file_lock = PTHREAD_MUTEX_INITIALIZER;

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
		if (g_table)
		{
			kv_free_table(g_table);
			g_table = NULL;
		}
		return (1);
	}
	g_log_file = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (g_log_file == -1)
	{
		perror("failed to open log file\n");
		pthread_join(autosave_thread, NULL);
		server_config_destroy(config);
		if (g_table)
		{
			kv_free_table(g_table);
			g_table = NULL;
		}
		return (1);
	}
	printf("LOG FILE OK\n");
	if (server_start(config) != 0)
	{
		fprintf(stderr, "Server failed to start\n");
		pthread_join(autosave_thread, NULL);
		server_config_destroy(config);
		if (g_table)
		{
			kv_free_table(g_table);
			g_table = NULL;
		}
		return (1);
	}
	// server stop
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