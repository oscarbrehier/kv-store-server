#include "globals.h"
#include "common.h"
#include "server.h"

void	ssl_ctx_cleanup(t_server_config *config)
{
	if (config && config->ssl_ctx)
	{
		SSL_CTX_free(config->ssl_ctx);
		config->ssl_ctx = NULL;
		EVP_cleanup();
	}
}

int	ssl_ctx_init(t_server_config *config, const char *cert_file, const char *key_file)
{
	if (!config || !cert_file)
		return (-1);
	if (!key_file)
	{
		printf("no key file\n");
		return (-1);
	}
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	SSL_library_init();
	config->ssl_ctx = SSL_CTX_new(TLS_server_method());
	if (!config->ssl_ctx)
	{
		fprintf(stderr, "Failed to create SSL context: ");
		ERR_print_errors_fp(stderr);
		return (-1);
	}
	if (SSL_CTX_use_certificate_file(config->ssl_ctx, cert_file, SSL_FILETYPE_PEM) <= 0)
	{
		fprintf(stderr, "Failed to load certificate file '%s': ", cert_file);
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(config->ssl_ctx);
		config->ssl_ctx = NULL;
		return (-1);
	}
	if (SSL_CTX_use_PrivateKey_file(config->ssl_ctx, key_file, SSL_FILETYPE_PEM) <= 0)
	{
		fprintf(stderr, "Failed to load private key file '%s': ", key_file);
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(config->ssl_ctx);
		config->ssl_ctx = NULL;
		return (-1);
	}
	if (!SSL_CTX_check_private_key(config->ssl_ctx))
	{
		fprintf(stderr, "Private key does not match the certificate\n");
		SSL_CTX_free(config->ssl_ctx);
		config->ssl_ctx = NULL;
		return (-1);
	}
	return (0);
}