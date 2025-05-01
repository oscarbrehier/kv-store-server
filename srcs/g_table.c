#include "globals.h"
#include "kv_table.h"

t_kv_table *g_table = NULL;

int init_global_table(void)
{
    if (kv_init_table(&g_table, 8) != SUCCESS_CODE ||
        kv_load_file(g_table, "./data/bonjour.kvdb"))
    {
        return (1);
    }
    return (0);
}