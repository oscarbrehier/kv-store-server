#include "globals.h"
#include "common.h"
#include "kv_table.h"
#include "auth.h"
#include "status_codes.h"
#include "logs.h"

int	generate_random_salt(unsigned char *salt_bytes, int salt_length)
{
	if (RAND_bytes(salt_bytes, salt_length) != 1)
	{
		return (-1);
	}
	return (0);
}

void	compute_password_hash(const char *password, const unsigned char *salt_bytes, int salt_length, unsigned char *computed_hash)
{
	EVP_MD_CTX		*context;

	context = EVP_MD_CTX_new();
	if (!context)
		return ;
	// SHA256_Init(&context);
	// SHA256_Update(&context, salted_password, salt_length + strlen(password));
	// SHA256_Final(computed_hash, &context);
	EVP_DigestInit(context, EVP_sha256());
	EVP_DigestUpdate(context, salt_bytes, salt_length);
	EVP_DigestUpdate(context, password, strlen(password));
	EVP_DigestFinal(context, computed_hash, NULL);
	EVP_MD_CTX_free(context);
}

t_status	auth_register_user(const char *username, const char *password)
{
	t_status		status;
	unsigned char	computed_hash[SHA256_DIGEST_LENGTH];
	unsigned char	salt_bytes[SALT_SIZE];
	unsigned char	hashed_password_with_salt[SHA256_DIGEST_LENGTH + sizeof(salt_bytes)];
	char			*output = NULL;

	status = kv_get(auth_store, username, (void *)&output, STRING);
	if (status.code == SUCCESS)
		return (status_create(-1, AUTH_USERNAME_TAKEN, LOG_ERROR));
	if (generate_random_salt(salt_bytes, sizeof(salt_bytes)) == -1)
		return (status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR));
    compute_password_hash(password, salt_bytes, sizeof(salt_bytes), computed_hash);
	memcpy(hashed_password_with_salt, computed_hash, SHA256_DIGEST_LENGTH);
	memcpy(hashed_password_with_salt + SHA256_DIGEST_LENGTH, salt_bytes, sizeof(salt_bytes));
	if (kv_set(auth_store, username, (void *)hashed_password_with_salt, sizeof(hashed_password_with_salt), STRING).exit != 0)
		return (status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR));
	if (auth_store_save() == -1)
		return (status_create(-1, AUTH_INTERNAL_ERROR, LOG_ERROR));
	return (status_create(0, SUCCESS, LOG_INFO));
}

t_status	auth_login_user(const char *username, const char *password)
{
	t_status		status;
	unsigned char	salt_bytes[SALT_SIZE];
	unsigned char	stored_hash_and_salt[SHA256_DIGEST_LENGTH + SALT_SIZE];
	unsigned char	computed_hash[SHA256_DIGEST_LENGTH];

	status = kv_get(auth_store, username, (void *)&stored_hash_and_salt, STRING);
	if (status.code != SUCCESS)
		return (status_create(-1, AUTH_INVALID_CREDENTIALS, LOG_ERROR));
	memcpy(salt_bytes, stored_hash_and_salt + SHA256_DIGEST_LENGTH, SALT_SIZE);
	compute_password_hash(password, salt_bytes, sizeof(salt_bytes), computed_hash);
	if (memcmp(stored_hash_and_salt, computed_hash, SHA256_DIGEST_LENGTH) != 0)
	{
		return (status_create(-1, AUTH_INVALID_CREDENTIALS, LOG_ERROR));
	}
	return (status_create(0, SUCCESS, LOG_INFO));
}