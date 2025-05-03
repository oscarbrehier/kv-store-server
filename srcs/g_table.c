#include "globals.h"
#include "kv_table.h"

t_kv_table	*g_table = NULL;
int			is_dirty = 0;

int init_global_table(void)
{
    if (kv_init_table(&g_table, 8) != SUCCESS_CODE ||
        kv_load_file(g_table, "./data/bonjour.kvdb"))
    {
        return (1);
    }
    return (0);
}

void    *g_table_autosave(void *arg)
{
    t_status_code   status;
    int             retry;

    (void)arg;
    retry = 0;
    while (1)
    {
        sleep(10);
		if (is_dirty == 0)
		{
			printf("(autosave) no data was modified\n");
            fflush(stdout);
			continue;
		}
        retry = 0;
        status = kv_save_file(g_table, "./data/bonjour.kvdb");
        while (status != SUCCESS_CODE && retry < MAX_AUTOSAVE_RETRY)
        {
            printf("(error) autosave failure. retrying.\n");
            fflush(stdout);
            status = kv_save_file(g_table, "./data/bonjour.kvdb");
            retry++;
        }
        if (status == SUCCESS_CODE)
        {
            is_dirty = 0;
            printf("(autosave) OK\n");
        }
        else
            printf("(autosave) FAILED after %d retries\n", retry);
        fflush(stdout);
    }
    return (NULL);
}