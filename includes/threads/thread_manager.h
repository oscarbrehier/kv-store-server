#ifndef THREADS_H
# define THREADS_H

#include <pthread.h>

int	    initialize_thread_resources(void);
int	    resize_thread_array();
int	    add_thread_to_array(pthread_t tid);
void	remove_thread_from_array(pthread_t tid);
void	wait_for_threads(void);

#endif