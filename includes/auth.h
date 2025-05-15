#ifndef AUTH_H
# define AUTH_H

#define SALT_SIZE 32

typedef struct s_kv_table t_kv_table;
typedef struct s_status t_status;

extern t_kv_table	*auth_store;

int			auth_sys_init();
int			auth_store_save(void);
void		auth_sys_cleanup(void);
t_status	auth_register_user(const char *username, const char *password);
t_status	auth_login_user(const char *username, const char *password);

#endif