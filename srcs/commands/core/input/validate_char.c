#include "globals.h"
#include "common.h"
#include "commands.h"

int	input_is_valid_char(char c)
{
	if (
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9') ||
		c == '_' ||
		c == '-' ||
		c == ':' ||
		c == ' '
	)
	{
		return (FN_SUCCESS);
	}
	return (FN_ERROR);
}