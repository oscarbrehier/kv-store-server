#ifndef GLOBALS_H
# define GLOBALS_H

#include <signal.h>
#include <pthread.h>

#define PORT 8080
#define DEFAULT_BUFFER_SIZE 1024
#define MAX_AUTOSAVE_RETRY 1

extern volatile sig_atomic_t	should_exit;
extern pthread_t				*thread_ids;
extern pthread_t				autosave_thread;
extern int						max_threads;
extern int						num_threads;
extern pthread_mutex_t 		    thread_mutex;


#endif