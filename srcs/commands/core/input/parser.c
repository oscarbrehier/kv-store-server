#include "globals.h"
#include "common.h"
#include "libs.h"
#include "client.h"
#include "commands.h"
#include "utils/dynamic_buffer.h"

int	resize_argv_if_needed(char ***argv, int *arg_capacity, int arg_count)
{
	int		i;
	int		old_capacity;
    char	**temp;
            
    if (arg_count > *arg_capacity - 1)
	{
		old_capacity = *arg_capacity;
		*arg_capacity *= 2;
		temp = realloc(*argv, sizeof(char *) * (*arg_capacity + 1));
		if (!temp)
			return (0);
		*argv = temp;
		i = old_capacity + 1;
		while (i <= *arg_capacity)
		{
			(*argv)[i] = NULL;
			i++;
		}
	}
    return (1);
}

void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

int	process_quoted_arg(char *input, int i, int *arg_length, char quote)
{
	i++;
	while (input[i])
	{
		if (input[i] == '\\' && input[i+1]) {
			i += 2;
			(*arg_length)++;
			continue;
		}
		if (input[i] == quote)
			break;
		(*arg_length)++;
		i++;
	}
	if (!input[i])
		return (-1);
	i++;
	return (i);
}

int	process_arg(char *input, int i, int *arg_length)
{
	while (input[i] && !is_space(input[i]))
	{
		(*arg_length)++;
		i++;
	}
	return (i);
}

void	copy_args(char *arg, const char *input, int start, int arg_length, int is_quoted)
{
	int src_idx;
	int dst_idx;
	
	src_idx = is_quoted ? start + 1 : start;
	dst_idx = 0;
	while (dst_idx < arg_length)
	{
		if (input[src_idx] == '\\' && input[src_idx + 1]) {
			arg[dst_idx] = input[src_idx+1];
			src_idx += 2;
		}
		else
		{
			arg[dst_idx] = input[src_idx];
			src_idx++;
		}
		dst_idx++;
	}
	arg[dst_idx] = '\0';
}

int	handle_argument(char ***argv, int arg_count, char *input, int *i, t_dynamic_buffer **buffer)
{
	int		j;
	int		start_pos;
	int		arg_length;
	int		is_quoted;
	char    quote;

	start_pos = *i;
	arg_length = 0;
	is_quoted = 0;
	if (is_quote(input[*i]))
	{
		is_quoted = 1;
		quote = input[*i];
		*i = process_quoted_arg(input, *i, &arg_length, quote);
	}
	else
		*i = process_arg(input, *i, &arg_length);

	(*argv)[arg_count] = malloc(sizeof(char) * (arg_length + 1));
	if (!(*argv)[arg_count])
	{
		free_argv(*argv);
		return (FN_ERROR);
	}
	copy_args((*argv)[arg_count], input, start_pos, arg_length, is_quoted);
	j = 0;
	while (j < arg_length)
	{
		if (input_is_valid_char((*argv)[arg_count][j]) == FN_ERROR)
		{
			dynamic_buffer_append(*buffer, "input contains invalid characters\n", -1);
			free((*argv)[arg_count]);
			(*argv)[arg_count] = NULL;
			return (FN_ERROR);
		}
		j++;
	}
	return (FN_SUCCESS);
}

int	input_parse(char *input, int *argc, char ***argv, t_dynamic_buffer **buffer)
{
    int i;
    int arg_capacity;
    int arg_count;
    
    arg_capacity = 8;
    *argv = malloc(sizeof(char *) * (arg_capacity + 1));
    if (!*argv)
    {
		return (FN_ERROR);
	}
	i = 0;
	while (i <= arg_capacity)
	{
		(*argv)[i] = NULL;
		i++;
	}
    arg_count = 0;
    i = 0;
    while (input[i])
    {
        while (input[i] && is_space(input[i]))
            i++;
        if (!input[i])
            break;
        if (!resize_argv_if_needed(argv, &arg_capacity, arg_count))
		{
			free_argv(*argv);
			return (FN_ERROR);
		}
        
		if (handle_argument(argv, arg_count, input, &i, buffer) == FN_ERROR)
		{
			free_argv(*argv);
			return (FN_ERROR);
		}
        arg_count++;
    }
    (*argv)[arg_count] = NULL;
    *argc = arg_count;
	return (FN_SUCCESS);
}