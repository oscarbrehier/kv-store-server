#ifndef CLIENT_H
# define CLIENT_H

#include "common.h"

typedef struct s_rate_limiter
{
    double	        tokens;
	double	        max_tokens;
	double	        refill_rate;
	time_t	        last_refill;
    pthread_mutex_t lock;
} t_rate_limiter;

typedef struct s_client
{
    int         	socket;
    char        	ip[INET_ADDRSTRLEN];
    uint16_t    	port;
    int             authenticated;
    t_rate_limiter	rate_limiter;
    SSL             *ssl;
} t_client;

void	initialize_client(t_client *client, int socket, const char *ip, uint16_t port);
void	*client_handler(void *arg);
void	ratelimit_init(t_client *client);
int		allow_request(t_client *client);

#endif