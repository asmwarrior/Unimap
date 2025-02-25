/*
	sslserver.c

	Example stand-alone SSL-secure gSOAP Web service.

	SSL-enabled services use the gSOAP SSL interface. See sslclient.c and
	sslserver.c for example code with instructions and the gSOAP
	documentation more details.

--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2001-2008, Robert van Engelen, Genivia, Inc. All Rights Reserved.
This software is released under one of the following two licenses:
GPL or Genivia's license for commercial use.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#include "soapH.h"
#include "ssl.nsmap"
#include <unistd.h>		/* defines _POSIX_THREADS if pthreads are available */
#if defined(_POSIX_THREADS) || defined(_SC_THREADS)
#include <pthread.h>
#endif
#include <signal.h>		/* defines SIGPIPE */

/******************************************************************************\
 *
 *	Forward decls
 *
\******************************************************************************/

void *process_request(void*);
int CRYPTO_thread_setup();
void CRYPTO_thread_cleanup();
void sigpipe_handle(int);

/******************************************************************************\
 *
 *	Main
 *
\******************************************************************************/

int main()
{ int m, s; /* master and slave sockets */
#if defined(_POSIX_THREADS) || defined(_SC_THREADS)
  pthread_t tid;
#endif
  struct soap soap, *tsoap;
  /* Need SIGPIPE handler on Unix/Linux systems to catch broken pipes: */
  signal(SIGPIPE, sigpipe_handle);
  if (CRYPTO_thread_setup())
  { fprintf(stderr, "Cannot setup thread mutex\n");
    exit(1);
  }
  /* init gsoap context and OpenSSL */
  soap_init(&soap);
  if (soap_ssl_server_context(&soap,
    SOAP_SSL_DEFAULT,	/* use SOAP_SSL_REQUIRE_CLIENT_AUTHENTICATION to verify clients: client must provide a key file e.g. "client.pem" and "password" */
    "server.pem",	/* keyfile: see SSL docs on how to obtain this file */
    "password",		/* password to read the key file */
    "cacert.pem",	/* cacert file to store trusted certificates (to authenticate clients) */
    NULL,		/* capath */
    "dh512.pem",	/* DH file name or DH param key len bits (e.g. "1024"), if NULL use RSA */
    NULL,		/* if randfile!=NULL: use a file with random data to seed randomness */ 
    "sslserver"		/* server identification for SSL session cache (unique server name, e.g. use argv[0]) */
  ))
  { soap_print_fault(&soap, stderr);
    exit(1);
  }
  soap.accept_timeout = 60;	/* server times out after 10 minutes of inactivity */
  soap.recv_timeout = 30;	/* if read stalls, then timeout after 60 seconds */
  m = soap_bind(&soap, NULL, 18081, 100);
  if (m < 0)
  { soap_print_fault(&soap, stderr);
    exit(1);
  }
  fprintf(stderr, "Bind successful: socket = %d\n", m);
  for (;;)
  { s = soap_accept(&soap);
    if (s < 0)
    { if (soap.errnum)
        soap_print_fault(&soap, stderr);
      else
        fprintf(stderr, "Server timed out\n");
      break;
    }
    fprintf(stderr, "Socket %d connection from IP %d.%d.%d.%d\n", s, (int)(soap.ip>>24)&0xFF, (int)(soap.ip>>16)&0xFF, (int)(soap.ip>>8)&0xFF, (int)soap.ip&0xFF);
    tsoap = soap_copy(&soap);
    if (!tsoap)
    { soap_closesock(&soap);
      continue;
    }
#if defined(_POSIX_THREADS) || defined(_SC_THREADS)
    pthread_create(&tid, NULL, &process_request, (void*)tsoap);
#else
    process_request((void*)tsoap);
#endif
  }
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap); /* MUST call before CRYPTO_thread_cleanup */
  CRYPTO_thread_cleanup();
  return 0;
} 

void *process_request(void *soap)
{
#if defined(_POSIX_THREADS) || defined(_SC_THREADS)
  pthread_detach(pthread_self());
#endif
  if (soap_ssl_accept(soap) != SOAP_OK)
  { /* when soap_ssl_accept() fails, socket is closed and SSL data reset */
    soap_print_fault(soap, stderr);
    fprintf(stderr, "SSL request failed, continue with next call...\n");
  }
  else
    soap_serve((struct soap*)soap);
  soap_destroy((struct soap*)soap); /* for C++ */
  soap_end((struct soap*)soap);
  soap_free((struct soap*)soap);
  return NULL;
}

/******************************************************************************\
 *
 *	Service methods
 *
\******************************************************************************/

int ns__add(struct soap *soap, double a, double b, double *result)
{ *result = a + b;
  return SOAP_OK;
} 

/******************************************************************************\
 *
 *	OpenSSL
 *
\******************************************************************************/

#ifdef WITH_OPENSSL

#if defined(WIN32)
# define MUTEX_TYPE		HANDLE
# define MUTEX_SETUP(x)		(x) = CreateMutex(NULL, FALSE, NULL)
# define MUTEX_CLEANUP(x)	CloseHandle(x)
# define MUTEX_LOCK(x)		WaitForSingleObject((x), INFINITE)
# define MUTEX_UNLOCK(x)	ReleaseMutex(x)
# define THREAD_ID		GetCurrentThreadId()
#elif defined(_POSIX_THREADS) || defined(_SC_THREADS)
# define MUTEX_TYPE		pthread_mutex_t
# define MUTEX_SETUP(x)		pthread_mutex_init(&(x), NULL)
# define MUTEX_CLEANUP(x)	pthread_mutex_destroy(&(x))
# define MUTEX_LOCK(x)		pthread_mutex_lock(&(x))
# define MUTEX_UNLOCK(x)	pthread_mutex_unlock(&(x))
# define THREAD_ID		pthread_self()
#else
# error "You must define mutex operations appropriate for your platform"
# error	"See OpenSSL /threads/th-lock.c on how to implement mutex on your platform"
#endif

struct CRYPTO_dynlock_value
{ MUTEX_TYPE mutex;
};

static MUTEX_TYPE *mutex_buf;

static struct CRYPTO_dynlock_value *dyn_create_function(const char *file, int line)
{ struct CRYPTO_dynlock_value *value;
  value = (struct CRYPTO_dynlock_value*)malloc(sizeof(struct CRYPTO_dynlock_value));
  if (value)
    MUTEX_SETUP(value->mutex);
  return value;
}

static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l, const char *file, int line)
{ if (mode & CRYPTO_LOCK)
    MUTEX_LOCK(l->mutex);
  else
    MUTEX_UNLOCK(l->mutex);
}

static void dyn_destroy_function(struct CRYPTO_dynlock_value *l, const char *file, int line)
{ MUTEX_CLEANUP(l->mutex);
  free(l);
}

void locking_function(int mode, int n, const char *file, int line)
{ if (mode & CRYPTO_LOCK)
    MUTEX_LOCK(mutex_buf[n]);
  else
    MUTEX_UNLOCK(mutex_buf[n]);
}

unsigned long id_function()
{ return (unsigned long)THREAD_ID;
}

int CRYPTO_thread_setup()
{ int i;
  mutex_buf = (MUTEX_TYPE*)malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
  if (!mutex_buf)
    return SOAP_EOM;
  for (i = 0; i < CRYPTO_num_locks(); i++)
    MUTEX_SETUP(mutex_buf[i]);
  CRYPTO_set_id_callback(id_function);
  CRYPTO_set_locking_callback(locking_function);
  CRYPTO_set_dynlock_create_callback(dyn_create_function);
  CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
  CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
  return SOAP_OK;
}

void CRYPTO_thread_cleanup()
{ int i;
  if (!mutex_buf)
    return;
  CRYPTO_set_id_callback(NULL);
  CRYPTO_set_locking_callback(NULL);
  CRYPTO_set_dynlock_create_callback(NULL);
  CRYPTO_set_dynlock_lock_callback(NULL);
  CRYPTO_set_dynlock_destroy_callback(NULL);
  for (i = 0; i < CRYPTO_num_locks(); i++)
    MUTEX_CLEANUP(mutex_buf[i]);
  free(mutex_buf);
  mutex_buf = NULL;
}

#endif

/******************************************************************************\
 *
 *	SIGPIPE
 *
\******************************************************************************/

void sigpipe_handle(int x) { }

