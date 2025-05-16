#include "globals.h"
#include "common.h"
#include "kv_table.h"
#include "auth.h"
#include "status_codes.h"
#include "logs.h"

t_kv_table	*auth_store = NULL;

int	auth_sys_init(void)
{
	t_status	status;

	if (kv_init_table(&auth_store, DEFAULT_AUTH_STORE_SIZE).exit == -1)
		return (-1);
	status = kv_load_file(auth_store, AUTH_STORE_PATH);
	if (status.exit == -1)
	{
		if (status.code == ERROR_FILE_OPEN || status.code == ERROR_FILE_HEADER)
		{
			if (kv_save_file(auth_store, AUTH_STORE_PATH).exit == -1)
				return (-1);
		}
		else
		{
			return (-1);
		}
	}
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