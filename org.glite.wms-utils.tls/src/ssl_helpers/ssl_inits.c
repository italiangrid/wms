#ident "$Header$"

#include <openssl/ssl.h>

#include "ssl_inits.h"
//#include "glite/wmsutils/thirdparty/globus_ssl_utils/sslutils.h"

int edg_wlc_SSLInitialization(void)
{
  SSL_CTX *ssl_context;

  /* Initialize OpenSSL library */

  SSL_load_error_strings();
  SSL_library_init();

  /* Initialize Globus ssl_utils library,
   * includes random number generator initialization */

  ERR_load_prxyerr_strings(1);

  /* preventing initialisation race in SSL_get_ex_data_X509_STORE_CTX_idx() */
  ssl_context = SSL_CTX_new(SSLv23_method());
  SSL_CTX_free(ssl_context);

  return 0;
}
