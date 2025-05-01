#ifndef G_TABLE_H
# define G_TABLE_H

typedef struct s_kv_table t_kv_table;

extern t_kv_table	*g_table;

int	init_global_table(void);

#endif