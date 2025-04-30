#include "commands.h"
#include "common.h"
#include "kv_table.h"

static t_kv_table	*command_table = NULL;

int	command_sys_init(void)
{
	kv_init_table(&command_table, 32);
	if (!command_table)
		return (1);
	return (0);
}

void	command_sys_cleanup(void)
{
	if (command_table)
	{
		kv_free_table(command_table);
		command_table = NULL;
	};
}

int	command_register(t_command *cmd)
{
	if (!command_table || !cmd || !cmd->name)
		return (1);
	if (kv_set(command_table, cmd->name, cmd, sizeof(t_command), STRING) != SUCCESS_CODE)
		return (1);
	return (0);
}

t_command	*command_find(const char *name)
{
	void	*command;

	if (!command_table || !name)
		return (NULL);
	if (kv_get(command_table, name, &command, STRING) != SUCCESS_CODE)
		return (NULL);
	return ((t_command *)command);
}

void    command_exec(int socket, t_kv_table *table, int argc, char **argv)
{
    t_status_code   status;
    char            *output;

    if (strcmp(argv[0], "get") == 0 && argc == 2)
    {
        status = kv_get(table, argv[1], (void *)&output, STRING);
        if (status == SUCCESS_CODE)
			client_send(socket, output);
        return ;
    }        
}

void    handle_client_input(int socket, t_kv_table *table, char *input)
{   
    int     argc;
    char    **argv;

    parse_input(input, &argc, &argv);
    if (!argc)
        return ;
	command_exec(socket, table, argc, argv);
}