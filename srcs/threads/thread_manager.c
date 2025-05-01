#include "common.h"
#include "globals.h"

pthread_t				*thread_ids = NULL;
pthread_t				autosave_thread;
int						max_threads = 0;
int						num_threads = 0;
pthread_mutex_t 		thread_mutex = PTHREAD_MUTEX_INITIALIZER;

int	initialize_thread_resources(void)
{
	max_threads = 10;
	thread_ids = malloc(sizeof(pthread_t) * max_threads);
	if (!thread_ids)
	{
		perror("Failed to allocate thread tracking");
		return (-1);
	}
	return (0);
}

int	resize_thread_array()
{
	pthread_t	*temp;
	int			new_max;

	new_max = num_threads * 2;
	temp = realloc(thread_ids, sizeof(pthread_t) * new_max);
	if (!temp)
	{
		perror("Failed to reallocate thread IDs");
		return (-1);
	}
	thread_ids = temp;
	max_threads *= 2;
	return (0);
}

int	add_thread_to_array(pthread_t tid)
{
	pthread_mutex_lock(&thread_mutex);
	if (num_threads >= max_threads)
    {
        if (resize_thread_array() != 0)
        {
            pthread_mutex_unlock(&thread_mutex);
            return (-1);
        }
    }
    if (num_threads < max_threads)
    {
        thread_ids[num_threads++] = tid;
        pthread_mutex_unlock(&thread_mutex);
        return (0);
    }
	pthread_mutex_unlock(&thread_mutex);
	return (-1);
}

void	remove_thread_from_array(pthread_t tid)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&thread_mutex);
	while (i < num_threads)
	{
		if (pthread_equal(thread_ids[i], tid))
		{
			if (i < num_threads - 1)
			{
				thread_ids[i] = thread_ids[num_threads - 1];
			}
			num_threads--;
			break ;
		}
		i++;
	}
	pthread_mutex_unlock(&thread_mutex);
}

void	wait_for_threads(void)
{
	int	i;

	if (should_exit)
	{
		pthread_mutex_lock(&thread_mutex);
		i = 0;
		while (i < num_threads)
		{
			pthread_cancel(thread_ids[i]);
			i++;
		}
		pthread_mutex_unlock(&thread_mutex);
		pthread_cancel(autosave_thread);
		pthread_join(autosave_thread, NULL);
	}
}
