#ifndef LOGS_H
# define LOGS_H

#include "common.h"
#include "status_codes.h"

#define LOG_FILE "./.logs/server.log"

typedef struct s_client t_client;

extern int				g_log_file;
extern pthread_mutex_t	log_file_lock;

char	*get_log_level(int level);
void    alog(int log_level, const char *address, uint16_t port, const char *message);
void    alogf(int log_level, const char *address, uint16_t port, char *message, ...);

#endif