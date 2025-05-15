#ifndef AUTH_H
# define AUTH_H

typedef struct s_kv_table t_kv_table;
typedef struct s_status t_status;

extern t_kv_table	*auth_table;

t_status	auth_register(const char *username, const char *password);
int			auth_sys_init();
void		auth_sys_cleanup(void);

#endif