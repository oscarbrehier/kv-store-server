#ifndef GLOBALS_H
# define GLOBALS_H

#include <signal.h>
#include <pthread.h>

#define FN_ERROR -1
#define FN_SUCCESS 0

#define CONFIG_BASE_PATH "./config/"
#define CERT_FILE CONFIG_BASE_PATH "server.crt"
#define KEY_FILE CONFIG_BASE_PATH "server.key"

#define DATA_BASE_PATH "./data/"
#define KV_STORE_PATH DATA_BASE_PATH "bonjour.kvdb"
#define AUTH_STORE_PATH DATA_BASE_PATH "authentication.kvdb"

#define MAX_SAVE_RETRY 1
#define MAX_EVENTS 64
#define MAX_BUFFER_SIZE 4096
#define DEFAULT_THREAD_COUNT 4
#define MAX_QUEUE_SIZE 1024
#define RATE_LIMIT 5
#define MAX_REQ_TOKENS 1

#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 10
#define DEFAULT_MAX_CLIENTS 100
#define DEFAULT_THREAD_POOL_SIZE 4
#define DEFAULT_AUTH_STORE_SIZE 16

#define ERROR_LEN 128

typedef struct s_server_config t_server_config;

extern volatile int running;
extern t_server_config	*config;

#endif