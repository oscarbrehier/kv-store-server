#ifndef THREAD_POOL_H
# define THREAD_POOL_H

#include "common.h"

typedef struct s_kv_table t_kv_table;

typedef struct s_thread
{
    pthread_t	thread_id;
	int			is_active;
} t_thread;

typedef struct s_thread_pool
{
	t_thread		*threads;
	int				size;
	int				max_size;
	int				shutdown;
	int				*client_sockets;
	int				client_count;
	pthread_mutex_t	lock;
	pthread_cond_t	condition;
	t_kv_table		*table;
} t_thread_pool;

t_thread_pool	*thread_pool_create(int size, t_kv_table *table);
void			thread_pool_destroy(t_thread_pool *pool);
void			thread_pool_add_client(t_thread_pool *pool, int client_socket);

#endif