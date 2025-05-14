#include "common.h"
#include "globals.h"
#include "thread_pool.h"
#include "client.h"
#include "server.h"
#include "utils/dynamic_buffer.h"
#include "commands.h"
#include "logs.h"

int	handle_input(t_dynamic_buffer *recv_buffer, t_dynamic_buffer *send_buffer, int *client_active, t_client client)
{
	char	*input;
	char	**argv;
	int		argc;

	input = strtok(recv_buffer->buffer, "\t\r\n");
	if (!input)
	{
		return (0);
	}
	if (strcmp(input, "quit") == 0)
	{
		*client_active = 0;
		alog(LOG_INFO, client.ip, client.port, "client disconnected");
		return (1);
	}
	parse_input(input, &argc, &argv);
	command_exec(&send_buffer, argc, argv, client);
	if (argv)
	{
		free_argv(argv);
	}
	return (0);
}

void serve_client(t_dynamic_buffer *recv_buffer, t_dynamic_buffer *send_buffer, t_client client)
{
	char	recv_chunk[1024];
	ssize_t	bytes_read;
	int		client_active;

	client_active = 1;
	while (client_active && running)
	{
		dynamic_buffer_reset(recv_buffer);
		dynamic_buffer_reset(send_buffer);
		while ((bytes_read = recv(client.socket, recv_chunk, sizeof(recv_chunk) - 1, 0)) > 0)
		{
			recv_chunk[bytes_read] = '\0';
			dynamic_buffer_append(recv_buffer, recv_chunk, bytes_read);
			if (strchr(recv_chunk, '\n') != NULL)
				break;
		}
		if (bytes_read <= 0)
		{
			client_active = 0;
			continue;
		}
		if (handle_input(recv_buffer, send_buffer, &client_active, client) == 1)
		{
			break ;
		}
		if (send_buffer->used > 0)
		{
			send(client.socket, send_buffer->buffer, send_buffer->used, 0);
		}
	}
}

void *client_handler(void *arg)
{
	int 				i;
	int					client_socket;
	t_thread_pool		*pool;
	t_dynamic_buffer	*recv_buffer;
	t_dynamic_buffer	*send_buffer;

	pool = (t_thread_pool *)arg;
	recv_buffer = dynamic_buffer_create(4096);
	send_buffer = dynamic_buffer_create(4096);
	if (!recv_buffer || !send_buffer)
	{
		if (recv_buffer)
			dynamic_buffer_destroy(recv_buffer);
		if (send_buffer)
			dynamic_buffer_destroy(send_buffer);
		fprintf(stderr, "Failed to allocate buffer for client handler\n");
		return (NULL);
	}
	while (!pool->shutdown)
	{
		pthread_mutex_lock(&pool->lock);
		while (pool->client_count == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->condition, &pool->lock);
		}
		if (pool->shutdown)
		{
			pthread_mutex_unlock(&pool->lock);
			break;
		}
		client_socket = pool->clients[0].socket;
		i = 0;
		while (i < pool->client_count - 1)
		{
			pool->clients[i] = pool->clients[i + 1];
			i++;
		}
		pool->client_count--;
		pthread_mutex_unlock(&pool->lock);
		if (client_socket > 0)
		{
			serve_client(recv_buffer, send_buffer, pool->clients[0]);
		}
		send(client_socket, "Goodbye!\n", strlen("Goodbye!\n"), 0);
		close(client_socket);
	}
	dynamic_buffer_destroy(recv_buffer);
	dynamic_buffer_destroy(send_buffer);
	return (NULL);
}