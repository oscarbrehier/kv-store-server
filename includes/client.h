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
    t_rate_limiter	rate_limiter;
} t_client;

void	*client_handler(void *arg);
void	ratelimit_init(t_client *client);
int		allow_request(t_client *client);

#endif