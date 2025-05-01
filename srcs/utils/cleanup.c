#include "common.h"
#include "globals.h"
#include "g_table.h"
#include "commands.h"
#include "kv_table.h"

void cleanup_ressources()
{
	pthread_mutex_lock(&thread_mutex);
	free(thread_ids);
	thread_ids = NULL;
	pthread_mutex_unlock(&thread_mutex);
	pthread_mutex_destroy(&thread_mutex);
	command_sys_cleanup();
	kv_free_table(g_table);
}