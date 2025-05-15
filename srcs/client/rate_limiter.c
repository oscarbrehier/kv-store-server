#include "globals.h"
#include "common.h"
#include "client.h"

void	ratelimit_init(t_client *client)
{
	t_rate_limiter	rate_limiter;

	if (!client)
		return ;
	rate_limiter.tokens = MAX_REQ_TOKENS;
	rate_limiter.max_tokens = MAX_REQ_TOKENS;
	rate_limiter.refill_rate = RATE_LIMIT;
	rate_limiter.last_refill = time(NULL);
	client->rate_limiter = rate_limiter;
	pthread_mutex_init(&client->rate_limiter.lock, NULL);
}

static void	refill(t_rate_limiter *rl)
{
	time_t	now;
	double	elapsed;
	double	tokens_to_add;

	now = time(NULL);
	elapsed = difftime(now, rl->last_refill);
	tokens_to_add = elapsed * rl->refill_rate;
	if (tokens_to_add > 0)
	{
		rl->tokens = rl->tokens + tokens_to_add;
		if (rl->tokens > rl->max_tokens)
			rl->tokens = rl->max_tokens;
		rl->last_refill = now;
	}
}

int	allow_request(t_client *client)
{	
	int	allowed;

	pthread_mutex_lock(&client->rate_limiter.lock);
	refill(&client->rate_limiter);
	if (client->rate_limiter.tokens >= 1)
	{
		client->rate_limiter.tokens -= 1;
		allowed = 1;
	}
	else
	{
		allowed = 0;
	}
	pthread_mutex_unlock(&client->rate_limiter.lock);
	return (allowed);
}