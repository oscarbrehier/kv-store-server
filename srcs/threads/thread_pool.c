#define _GNU_SOURCE
#include "server.h"
#include "kv_table.h"
#include "thread_pool.h"
#include "client.h"
#include "logs.h"

t_thread_pool	*thread_pool_create(int size)
{
	t_thread_pool	*pool;
	int				i;

	if (size <= 0)
		return (NULL);
	pool = malloc(sizeof(t_thread_pool));
	if (!pool)
		return (NULL);
	pool->threads = malloc(sizeof(t_thread) * size);
	pool->clients = malloc(sizeof(t_client) * size * 2);
	if (!pool->threads || !pool->clients)
	{
		if (pool->threads) free(pool->threads);
		if (pool->clients) free(pool->clients);
		free(pool);
		return (NULL);
	}
	pool->size = 0;
	pool->max_size = size;
	pool->shutdown = 0;
	pool->client_count = 0;
	pool->active_clients = 0;
	pthread_mutex_init(&pool->lock, NULL);
	pthread_cond_init(&pool->condition, NULL);
	i = 0;
	while (i < size)
	{
		pool->threads[i].is_active = 1;
		if (pthread_create(&pool->threads[i].thread_id, NULL, client_handler, pool) != 0)
		{
			pool->threads[i].is_active = 0;
			continue ;
		}
		pool->size++;
		i++;
	}
	return (pool);
}

void	thread_pool_destroy(t_thread_pool *pool)
{
	int				i;

	printf("destroying threadpool\n");
	if (!pool)
		return ;
	pthread_mutex_lock(&pool->lock);
	pool->shutdown = 1;
	pthread_cond_broadcast(&pool->condition);
	pthread_mutex_unlock(&pool->lock);
	i = 0;
	printf("joining threads (size %d) \n", pool->size);
	while (i < pool->size)
	{
		printf("processing thread %d\n", i);
		if (pool->threads[i].is_active)
			pthread_join(pool->threads[i].thread_id, NULL);
		i++;
	}
	i = 0;
	printf("closing socket connections\n");
	while (i < pool->client_count)
	{
		if (pool->clients[i].socket > 0)
		{
			printf("attempting to close socket %d\n", i);
			shutdown(pool->clients[i].socket, SHUT_RDWR);
			close(pool->clients[i].socket);
			printf("closed socket %d\n", i);
		}
		i++;
	}
	pthread_mutex_destroy(&pool->lock);
	pthread_cond_destroy(&pool->condition);
	free(pool->threads);
	free(pool->clients);
	free(pool);
}

int	thread_pool_add_client(t_thread_pool *pool, int client_socket, const char *ip, uint16_t port)
{
	int	client_pos;

	if (!pool)
		return (-1);
	pthread_mutex_lock(&pool->lock);
	if (pool->client_count < pool->max_size * 2)
	{
		client_pos = pool->client_count++;
		pool->clients[client_pos].socket = client_socket;
		strncpy(pool->clients[client_pos].ip, ip, INET_ADDRSTRLEN);
		pool->clients[client_pos].port = port;
		alogf(LOG_INFO, pool->clients[client_pos].ip, pool->clients[client_pos].port, "client connected (active: %d)", pool->active_clients);
		pthread_cond_signal(&pool->condition);
		pthread_mutex_unlock(&pool->lock);
		return (client_pos);
	}
	close(client_socket);
	pthread_mutex_unlock(&pool->lock);
	return (-1);
}
