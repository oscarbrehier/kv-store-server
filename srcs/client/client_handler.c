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
	int		flags;

	client_active = 1;
	while (client_active && running)
	{
		dynamic_buffer_reset(recv_buffer);
		dynamic_buffer_reset(send_buffer);
		flags = fcntl(client.socket, F_GETFL, 0);
		fcntl(client.socket, F_SETFL, flags | O_NONBLOCK);
		while ((bytes_read = recv(client.socket, recv_chunk, sizeof(recv_chunk) - 1, 0)) > 0)
		{
			recv_chunk[bytes_read] = '\0';
			dynamic_buffer_append(recv_buffer, recv_chunk, bytes_read);
			if (strchr(recv_chunk, '\n') != NULL)
				break;
		}
		if (bytes_read <= 0)
		{	
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				if (!running)
					break ;
				usleep(100000);
				continue ;
			}
			client_active = 0;
			continue;
		}
		if (allow_request(&client))
		{
			if (handle_input(recv_buffer, send_buffer, &client_active, client) == 1)
				break ;
		}
		else
		{
			dynamic_buffer_append(send_buffer, "rate limit exceeded\n", strlen("rate limit exceeded\n"));
		}
		if (send_buffer->used > 0) 
		{
			send(client.socket, send_buffer->buffer, send_buffer->used, 0);
		}
	}
	fcntl(client.socket, F_SETFL, flags);
}

void *client_handler(void *arg)
{
	int 				i;
	int					client_socket;
	t_thread_pool		*pool;
	t_dynamic_buffer	*recv_buffer;
	t_dynamic_buffer	*send_buffer;
	t_client			client;

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
		client = pool->clients[0];
		ratelimit_init(&client);
		client_socket = client.socket;
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
			pthread_mutex_lock(&pool->lock);
			pool->active_clients++;
			pthread_mutex_unlock(&pool->lock);
			serve_client(recv_buffer, send_buffer, client);
			pthread_mutex_lock(&pool->lock);
			pool->active_clients--;
			pthread_mutex_unlock(&pool->lock);
		}
		send(client_socket, "Goodbye!\n", strlen("Goodbye!\n"), 0);
		alogf(LOG_INFO, client.ip, client.port, "client disconnected (active: %d)", pool->active_clients);
		close(client_socket);
	}
	dynamic_buffer_destroy(recv_buffer);
	dynamic_buffer_destroy(send_buffer);
	return (NULL);
}