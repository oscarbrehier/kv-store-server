#include "globals.h"
#include "common.h"
#include "kv_table.h"
#include "auth.h"
#include "status_codes.h"
#include "logs.h"

t_kv_table	*auth_table = NULL;

int	auth_sys_init(void)
{
	if (kv_init_table(&auth_table, 16).exit == 0)
		return (0);
	return (-1);
}

void	auth_sys_cleanup(void)
{
	if (auth_table)
	{
		kv_free_table(auth_table);
		auth_table = NULL;
	}
}

int	generate_salt(unsigned char *salt, int salt_length)
{
	if (RAND_bytes(salt, salt_length) != 1)
	{
		return (-1);
	}
	return (0);
}

void	hash_salted_password(const char *password, const unsigned char *salt, int salt_length, unsigned char *hash)
{
	EVP_MD_CTX	*context;
	unsigned char	salted_password[SHA256_DIGEST_LENGTH];

	context = EVP_MD_CTX_new();
	memcpy(salted_password, salt, salt_length);
	strcpy((char *)(salted_password + salt_length), password);
	// SHA256_Init(&context);
	// SHA256_Update(&context, salted_password, salt_length + strlen(password));
	// SHA256_Final(hash, &context);
	EVP_DigestInit(context, EVP_sha256());
	EVP_DigestUpdate(context, salted_password, salt_length + strlen(password));
	EVP_DigestFinal(context, hash, NULL);
	EVP_MD_CTX_free(context);
}

t_status	auth_register(const char *username, const char *password)
{
	t_status		status;
	unsigned char	hash[SHA256_DIGEST_LENGTH];
	unsigned char	salt[32];
	unsigned char	hash_with_salt[SHA256_DIGEST_LENGTH + sizeof(salt)];
	char	*output = NULL;

	status = kv_get(auth_table, username, (void *)&output, STRING);
	if (status.code == SUCCESS)
	{
		status = status_create(-1, AUTH_USERNAME_TAKEN, LOG_ERROR);
		return (status);
	}
	if (generate_salt(salt, sizeof(salt)) == -1)
	{
		status = status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR);
		return (status);
	}
    hash_salted_password(password, salt, sizeof(salt), hash);
	memcpy(hash_with_salt, hash, SHA256_DIGEST_LENGTH);
	memcpy(hash_with_salt + SHA256_DIGEST_LENGTH, salt, sizeof(salt));
	if (kv_set(auth_table, username, (void *)hash_with_salt, sizeof(hash_with_salt), STRING).exit != 0)
	{
		status = status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR);
		return (status);
	}
	status = status_create(0, SUCCESS, LOG_INFO);
	return (status);
}