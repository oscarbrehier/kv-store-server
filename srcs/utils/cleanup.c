#include "common.h"
#include "globals.h"
#include "g_table.h"
#include "commands.h"
#include "kv_table.h"
#include "threads/thread_manager.h"

void cleanup_ressources()
{
	wait_for_threads();
	pthread_mutex_destroy(&thread_mutex);
	free(thread_ids);
	thread_ids = NULL;
	command_sys_cleanup();
	kv_free_table(g_table);
}