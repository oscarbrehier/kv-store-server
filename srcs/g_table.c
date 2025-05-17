#include "globals.h"
#include "kv_table.h"
#include "logs.h"
#include "status_codes.h"

t_kv_table	*g_table = NULL;
int			is_dirty = 0;

int init_global_table(void)
{
    t_status    status;

    if (kv_init_table(&g_table, 8).code != SUCCESS)
        return (1);
    status =  kv_load_file(g_table, KV_STORE_PATH);
    if (status.exit == -1)
    {
        if (status.code == ERROR_FILE_OPEN || status.code == ERROR_FILE_HEADER)
        {
            if (kv_save_file(g_table, KV_STORE_PATH).exit == -1)
                return (-1);        
        }
        else
        {
            return (-1);
        }
    }
    return (0);
}

void    *g_table_autosave(void *arg)
{
    t_status	status;
    int			retry;

    (void)arg;
    retry = 0;
    printf("[OK] autosave\n");
    while (running)
    {
        sleep(10);
		if (is_dirty == 0)
		{
            fflush(stdout);
			continue;
		}
        retry = 0;
        status = kv_save_file(g_table, KV_STORE_PATH);
        while (status.code != SUCCESS && retry < MAX_SAVE_RETRY)
        {
            alog(LOG_ERROR, NULL, 0, "Autosave failed. Retrying");
            fflush(stdout);
            status = kv_save_file(g_table, KV_STORE_PATH);
            retry++;
        }
        if (status.code == SUCCESS)
        {
            alogf(LOG_INFO, NULL, 0, "Autosave trigger (%d change(s) since last save)", is_dirty);
            is_dirty = 0;
        }
        fflush(stdout);
    }
    return (NULL);
}