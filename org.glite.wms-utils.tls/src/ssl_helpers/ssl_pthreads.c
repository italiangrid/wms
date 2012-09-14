#ident "$Header$"

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#include <openssl/crypto.h>

#include "ssl_pthreads.h"

static void edg_wlc_SSLLockingCallback(int mode, int n, const char *file, int line);
static unsigned long edg_wlc_SSLThreadId(void);

static int num_locks = -1;
static pthread_mutex_t *locks = NULL;

int edg_wlc_SSLLockingInit(void)
{
  int i, ret;

  num_locks = CRYPTO_num_locks();
  assert(num_locks >= 0);

  locks = malloc(num_locks * sizeof(pthread_mutex_t));
  if (locks == NULL) return ENOMEM;

  for ( i = 0 ; i < num_locks; i++) {
    ret = pthread_mutex_init(locks + i, NULL);
    if (ret != 0) {
      for ( i-- ; i >= 0; i--) {
 	pthread_mutex_destroy(locks + i);
      }
      return ret;
    }
  }

  CRYPTO_set_id_callback(edg_wlc_SSLThreadId);
  CRYPTO_set_locking_callback(edg_wlc_SSLLockingCallback);

  return 0;
}

int edg_wlc_SSLLockingCleanup(void)
{
  int i;

  CRYPTO_set_locking_callback(NULL);

  assert(locks != NULL && num_locks >= 0);

  for ( i = 0 ; i < num_locks; i++) {
    pthread_mutex_destroy(locks + i);
  }

  free(locks);
  locks = NULL;
  num_locks = -2;

  return 0;
}

static void edg_wlc_SSLLockingCallback(int mode, int n, const char *file, int line)
{
  int ret;
  
  assert(0 <= n && n < num_locks);

  if (mode & CRYPTO_LOCK) {
    ret = pthread_mutex_lock(locks + n);
    assert(ret == 0);
  } else {
    ret = pthread_mutex_unlock(locks + n);
    assert(ret == 0);
  }
}

static unsigned long edg_wlc_SSLThreadId(void)
{
  return (unsigned long) pthread_self();
}
