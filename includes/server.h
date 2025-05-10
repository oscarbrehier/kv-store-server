
#ifndef SERVER_H
# define SERVER_H

typedef struct s_thread_pool t_thread_pool;
typedef struct s_kv_table t_kv_table;

typedef struct s_server_config
{
	int				port;
	int 			backlog;
	int 			max_clients;
	int				server_socket;
	int				running;
	t_thread_pool	*thread_pool;
} t_server_config;

t_server_config	*server_config_create(int port, int backlog, int max_clients, int thread_pool_size);
void			server_config_destroy(t_server_config *config);
void			server_stop(t_server_config *config);
int				server_start(t_server_config *config);

#endif