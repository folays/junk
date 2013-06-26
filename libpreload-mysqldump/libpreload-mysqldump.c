/*
 * Copyright (c) 2011
 *	     Eric Gouyer <folays@folays.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Compile with:
 * gcc -o libpreload-mysqldump.so -shared libpreload-mysqldump.c -fPIC -ldl
 * Use with:
 * LD_PRELOAD="`dirname $0`/libpreload-mysqldump.so" mysqldump ...
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define HOOK_LIB_PATH "/usr/lib/libmysqlclient.so.16"
#define HOOK_INIT(fname) static void *handle, (*f)(); if (!handle) handle = hook_init_ptr(&f, fname);

static void *hook_init_ptr(void (**f)(), const char *fname)
{
  static void *handle;

  handle = dlopen(HOOK_LIB_PATH, RTLD_LAZY);
  if (!handle)
    err(1, "dlopen %s", HOOK_LIB_PATH);
  *f = dlsym(handle, fname);
  if (!*f)
    err(1, "dlsym %s", fname);
  return handle;
}

void *mysql_real_connect(void *mysql, const char *host, const char *user, const char *passwd,
			 const char *db, unsigned int port, const char *unix_socket,
			 unsigned long clientflag)
{
  HOOK_INIT("mysql_real_connect");

  void *ret = ((void *(*)())f)(mysql, host, user, passwd, db, port, unix_socket, clientflag);
  mysql_query(mysql, "SET SQL_BIG_SELECTS=1");
  return ret;
}
