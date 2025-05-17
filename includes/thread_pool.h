#ifndef THREAD_POOL_H
# define THREAD_POOL_H

#include "common.h"

typedef struct s_kv_table t_kv_table;
typedef struct s_client t_client;

typedef struct s_thread
{
    pthread_t	thread_id;
	int			is_active;
	int			client_id;
} t_thread;

typedef struct s_thread_pool
{
	t_thread		*threads;
	int				size;
	int				max_size;
	int				shutdown;
	t_client		*clients;
	int				client_count;
	int				active_clients;
	pthread_mutex_t	lock;
	pthread_cond_t	condition;
} t_thread_pool;

t_thread_pool	*thread_pool_create(int size);
void			thread_pool_destroy(t_thread_pool *pool);
int				thread_pool_add_client(t_thread_pool *pool, int client_socket, const char *ip, uint16_t port, SSL *ssl);

#endif