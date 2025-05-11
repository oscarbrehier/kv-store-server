#include "globals.h"
#include "common.h"
#include "server.h"
#include "kv_table.h"
#include "signal_handler.h"
#include "thread_pool.h"
#include "server.h"
#include "g_table.h"
#include "commands.h"

t_server_config	*server_config_create(int port, int backlog, int max_clients, int thread_pool_size)
{
	config = malloc(sizeof(t_server_config));
	if (!config)
		return (NULL);
	config->port = port;
	config->backlog = backlog;
	config->max_clients = max_clients;
	config->server_socket = -1;
	config->running = 0;

	// INIT TABLE
	
	// if (init_global_table() != 0)
	// 	return (NULL);

	// config->table = g_table; // INIT OR CREATE TABLE HERE;

	// if (!config->table)
	// {
	// 	free(config);
	// 	return (NULL);
	// }
	config->thread_pool = thread_pool_create(thread_pool_size);
	if (!config->thread_pool)
	{
		free(config);
		return (NULL);
	}
	return (config);
}

void	server_config_destroy(t_server_config *config)
{
	if (!config)
		return ;
	if (config->server_socket >= 0)
		close(config->server_socket);
	if (config->thread_pool)
		thread_pool_destroy(config->thread_pool);
	free(config);
}

void	server_worker(t_server_config *config)
{
	struct sockaddr_in	client_addr;
	socklen_t			client_len;
	int					client_socket;

	while (running)
	{

		client_len = sizeof(client_addr);
		client_socket = accept(config->server_socket, (struct sockaddr *)&client_addr, &client_len);
		if (client_socket < 0)
		{
			if (errno == EINTR && !running)
				break ;
			perror("accept failed");
			continue ;
		}
		printf("new connection %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		thread_pool_add_client(config->thread_pool, client_socket);
	}
}

int	server_start(t_server_config *config)
{
	struct sockaddr_in	server_addr;
	int					opt;

	if (!config || config->running)
		return (-1);
	setup_signal();
	config->server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (config->server_socket < 0)
	{
		perror("Failed to create socket");
		return (-1);
	}
	opt = 1;
	if (setsockopt(config->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt failed");
		close(config->server_socket);
		config->server_socket = -1;
		return (-1);
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET,
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(config->port);
	if (bind(config->server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind failed");
		close(config->server_socket);
		config->server_socket = -1;
		return (-1);
	}
	if (listen(config->server_socket, config->backlog) < 0)
	{
		perror("listen failed");
		close(config->server_socket);
		config->server_socket = -1;
		return (-1);
	}
	if (command_sys_init() != 0)
	{
		perror("command registration failed");
		close(config->server_socket);
		config->server_socket = -1;
		return (-1);
	}
	printf("(server) listening on port %d\n", config->port);
	config->running = 1;
	running = 1;
	server_worker(config);
	printf("closing server\n");
	config->running = 0;
	command_sys_cleanup();
	close(config->server_socket);
	config->server_socket = -1;
	return (0);
}

void	server_stop(t_server_config *config)
{
	struct sockaddr_in	addr;
	int					sock;

	if (!config || !config->running)
		return ;
	running = 0;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config->port);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock >= 0)
	{
		connect(sock, (struct sockaddr *)&addr, sizeof(addr));
		close(sock);
	}
}
