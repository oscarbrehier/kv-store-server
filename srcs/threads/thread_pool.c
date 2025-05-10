#include "server.h"
#include "kv_table.h"
#include "thread_pool.h"
#include "client.h"

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
	pool->client_sockets = malloc(sizeof(int) * size * 2);
	if (!pool->threads || !pool->client_sockets)
	{
		if (pool->threads) free(pool->threads);
		if (pool->client_sockets) free(pool->client_sockets);
		free(pool);
		return (NULL);
	}
	pool->size = 0;
	pool->max_size = size;
	pool->shutdown = 0;
	pool->client_count = 0;
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
	int	i;

	if (!pool)
		return ;
	pthread_mutex_lock(&pool->lock);
	pool->shutdown = 1;
	pthread_cond_broadcast(&pool->condition);
	pthread_mutex_unlock(&pool->lock);
	i = 0;
	while (i < pool->size)
	{
		if (pool->threads[i].is_active)
			pthread_join(pool->threads[i].thread_id, NULL);
		i++;
	}
	i = 0;
	while (i < pool->client_count)
	{
		if (pool->client_sockets[i] > 0)
			close(pool->client_sockets[i]);
		i++;
	}
	pthread_mutex_destroy(&pool->lock);
	pthread_cond_destroy(&pool->condition);
	free(pool->threads);
	free(pool->client_sockets);
	free(pool);
}

void	thread_pool_add_client(t_thread_pool *pool, int client_socket)
{
	if (!pool)
		return ;
	pthread_mutex_lock(&pool->lock);
	if (pool->client_count < pool->max_size * 2)
	{
		pool->client_sockets[pool->client_count++] = client_socket;
		pthread_cond_signal(&pool->condition);
	}
	else
	{
		// Too many clients, close connection
		close(client_socket);
	}
	pthread_mutex_unlock(&pool->lock);
}
