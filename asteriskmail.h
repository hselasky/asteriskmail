/*-
 * Copyright (c) 2014 Hans Petter Selasky. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ASTERISKMAIL_H_
#define	_ASTERISKMAIL_H_

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sysexits.h>
#include <err.h>
#include <errno.h>
#include <netdb.h>
#include <libutil.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioccom.h>
#include <sys/filio.h>
#include <sys/uio.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define	ASTERISKMAIL_LINE_MAX 1024
#define	ASTERISKMAIL_STRING_MAX	128
#define	ASTERISKMAIL_BUF_MAX 4096
#define	ASTERISKMAIL_SOCK_MAX 32

struct am_message {
	TAILQ_ENTRY(am_message) entry;
	int	message_id;
	int	bytes;
	void   *data;
};

extern const int base64_get(const char **);
extern const char base64_get_iso8859_latin1(const char **);
extern const char *handle_read_line(FILE *io);
extern int handle_extract_receip(const char *, char *, int);
extern int handle_compare(const char *, const char *);
extern int handle_foreach_message(struct am_message **);
extern int handle_delete_message(struct am_message *);
extern int handle_insert_message(struct am_message *);
extern int handle_append_message(struct am_message *, uint8_t);
extern struct am_message *handle_create_message(void);
extern void handle_smtp_connection(int);
extern void handle_pop3_connection(int);
extern void handle_httpd_connection(int);
extern char hostname[128];
extern const char *am_username;
extern const char *am_password;

#endif					/* _ASTERISKMAIL_H_ */
