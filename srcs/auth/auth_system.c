#include "globals.h"
#include "common.h"
#include "kv_table.h"
#include "auth.h"
#include "status_codes.h"
#include "logs.h"

t_kv_table	*auth_store = NULL;

int	auth_sys_init(void)
{
	if (kv_init_table(&auth_store, 16).exit == -1 || kv_load_file(auth_store, "./data/authentication.kvdb").exit == -1)
		return (-1);
	return (0);
}

void	auth_sys_cleanup(void)
{
	if (auth_store)
	{
		kv_free_table(auth_store);
		auth_store = NULL;
	}
}

int	auth_store_save(void)
{
	int			retry;
	t_status 	status;

	retry = -1;
	status = status_create(-1, STATUS_UNKNOWN, LOG_NOLOG);
	while (status.code != SUCCESS && retry < MAX_SAVE_RETRY)
	{
		status = kv_save_file(auth_store, AUTH_STORE_PATH);
		retry++;
	}
	if (status.code == SUCCESS)
		return (0);
	alog(LOG_ERROR, NULL, 0, "authentication database save failure");
	return (-1);
}