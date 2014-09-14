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

#include "asteriskmail.h"

static struct pidfh *local_pid;
static char linebuffer[ASTERISKMAIL_LINE_MAX];
static TAILQ_HEAD(, am_message) head = TAILQ_HEAD_INITIALIZER(head);
static int do_fork;
static struct pollfd fds[ASTERISKMAIL_SOCK_MAX];
char	hostname[128];
const char *am_username = "asteriskmail";
const char *am_password;

const char *
handle_read_line(FILE *io)
{
	char buffer[2];
	int size = 0;

	if (fread(buffer, 1, 2, io) != 2)
		return (NULL);

	while (1) {
		if (buffer[0] == '\r' && buffer[1] == '\n')
			break;
		if (size == ASTERISKMAIL_LINE_MAX - 1)
			return (NULL);
		linebuffer[size++] = buffer[0];
		buffer[0] = buffer[1];
		if (fread(buffer + 1, 1, 1, io) != 1)
			return (NULL);
	}
	linebuffer[size++] = 0;
#if 0
	printf("GOT line: %s\n", linebuffer);
#endif
	return (linebuffer);
}

int
handle_compare(const char *line, const char *cmd)
{
	return (strncmp(line, cmd, strlen(cmd)));
}

int
handle_foreach_message(struct am_message **ppam)
{
	struct am_message *ptr = *ppam;

	if (ptr == NULL)
		ptr = TAILQ_FIRST(&head);
	else
		ptr = TAILQ_NEXT(ptr, entry);

	*ppam = ptr;

	return (ptr != NULL);
}

int
handle_delete_message(struct am_message *pam)
{
	if (pam->entry.tqe_prev != NULL)
		TAILQ_REMOVE(&head, pam, entry);

	free(pam->data);
	free(pam);
}

int
handle_insert_message(struct am_message *pam)
{
	TAILQ_INSERT_TAIL(&head, pam, entry);
}

int
handle_append_message(struct am_message *pam, uint8_t data)
{
	void *ptr;

	pam->bytes++;
	if (pam->bytes <= 0) {
		free(pam->data);
		pam->bytes = 0;
		return (1);
	}
	if ((pam->bytes & (pam->bytes - 1)) == 0) {
		ptr = pam->data;
		pam->data = realloc(ptr, 2 * pam->bytes);
		if (pam->data == NULL) {
			free(ptr);
			pam->bytes = 0;
			return (1);
		}
	}
	((uint8_t *)pam->data)[pam->bytes - 1] = data;
	return (0);
}

struct am_message *
handle_create_message(void)
{
	struct am_message *pam = malloc(sizeof(*pam));

	if (pam != NULL)
		memset(pam, 0, sizeof(*pam));
	return (pam);
}

static int
asteriskmail_do_listen(const char *host, const char *port, int buffer, struct pollfd *pfd, int num_sock)
{
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *res0;
	int error;
	int flag;
	int s;
	int ns = 0;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags |= AI_NUMERICHOST;

	if ((error = getaddrinfo(host, port, &hints, &res)))
		return (-1);

	res0 = res;

	do {
		if ((s = socket(res0->ai_family, res0->ai_socktype,
		    res0->ai_protocol)) < 0)
			continue;

		flag = 1;
		setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &flag, (int)sizeof(flag));
		setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &flag, (int)sizeof(flag));

		setsockopt(s, SOL_SOCKET, SO_SNDBUF, &buffer, (int)sizeof(buffer));
		setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffer, (int)sizeof(buffer));

		if (bind(s, res0->ai_addr, res0->ai_addrlen) == 0) {
			if (listen(s, 1) == 0) {
				if (ns < num_sock) {
					pfd[ns++].fd = s;
					continue;
				}
				break;
			}
		}
		close(s);
		s = -1;
	} while ((res0 = res0->ai_next) != NULL);

	freeaddrinfo(res);

	return (ns);
}

static void
asteriskmail_usage(void)
{
	fprintf(stderr,
	    "\n"
	    "\n" "asteriskmail - AsteriskMail v1.0, compiled %s %s"
	    "\n" "usage: asteriskmail [-B] [-b 127.0.0.1] [-p 25] [-P 110] [ -H 80] [-h]"
	    "\n" "       -B            run in background"
	    "\n" "       -b <addr>     bind address"
	    "\n" "       -p <port>     SMTP bind port"
	    "\n" "       -P <port>     POP3 bind port"
	    "\n" "       -H <port>     HTTPD bind port"
	    "\n" "       -h            show usage"
	    "\n"
	    __DATE__, __TIME__);
}

static void
do_exit(void)
{
	if (local_pid != NULL) {
		pidfile_remove(local_pid);
		local_pid = NULL;
	}
}

static int
do_pidfile(void)
{
	if (local_pid != NULL)
		return (0);

	local_pid = pidfile_open("/var/run/asteriskmail", 0600, NULL);
	if (local_pid == NULL) {
		return (EEXIST);
	} else {
		pidfile_write(local_pid);
	}
	return (0);
}

int
main(int argc, char **argv)
{
	const char *smtp_port = "25";
	const char *pop3_port = "110";
	const char *httpd_port = "80";
	const char *host = "127.0.0.1";
	int opt;
	int npop3;
	int nsmtp;
	int nhttpd;

	atexit(&do_exit);

	while ((opt = getopt(argc, argv, "b:p:P:BhH:")) != -1) {
		switch (opt) {
		case 'b':
			host = optarg;
			break;
		case 'p':
			smtp_port = optarg;
			break;
		case 'P':
			pop3_port = optarg;
			break;
		case 'H':
			httpd_port = optarg;
			break;
		case 'B':
			do_fork = 1;
			break;
		default:
			asteriskmail_usage();
			return (EX_USAGE);
		}
	}

	if (gethostname(hostname, sizeof(hostname)) == -1)
		errx(EX_SOFTWARE, "Cannot get hostname");

	if (do_pidfile()) {
		errx(EX_SOFTWARE, "Cannot create PID file. "
		    "CAPI server already running?");
	}
	if (do_fork) {
		if (daemon(0, 0) != 0)
			errx(EX_SOFTWARE, "Cannot daemonize");
	}
	nsmtp = asteriskmail_do_listen(host, smtp_port, ASTERISKMAIL_BUF_MAX,
	    fds, ASTERISKMAIL_SOCK_MAX);
	if (nsmtp < 1) {
		errx(EX_SOFTWARE, "Could not bind to "
		    "'%s' and '%s'\n", host, smtp_port);
	}
	npop3 = asteriskmail_do_listen(host, pop3_port, ASTERISKMAIL_BUF_MAX,
	    fds + nsmtp, ASTERISKMAIL_SOCK_MAX - nsmtp);
	if (npop3 < 1) {
		errx(EX_SOFTWARE, "Could not bind to "
		    "'%s' and '%s'\n", host, pop3_port);
	}
	nhttpd = asteriskmail_do_listen(host, httpd_port, ASTERISKMAIL_BUF_MAX,
	    fds + nsmtp + npop3, ASTERISKMAIL_SOCK_MAX - nsmtp - npop3);
	if (nhttpd < 1) {
		errx(EX_SOFTWARE, "Could not bind to "
		    "'%s' and '%s'\n", host, pop3_port);
	}
	while (1) {
		int ns = nsmtp + npop3 + nhttpd;
		int c;
		int f;

		for (c = 0; c != ns; c++) {
			fds[c].events = (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI |
			    POLLERR | POLLHUP | POLLNVAL);
			fds[c].revents = 0;
		}
		if (poll(fds, ns, -1) < 0)
			errx(EX_SOFTWARE, "Polling failed");

		for (c = 0; c != ns; c++) {
			if (fds[c].revents == 0)
				continue;
			f = accept(fds[c].fd, NULL, NULL);
			if (f < 0)
				continue;
			if (c < nsmtp)
				handle_smtp_connection(f);
			else if (c < nsmtp + npop3)
				handle_pop3_connection(f);
			else
				handle_httpd_connection(f);
			close(f);
		}
	}
	return (0);
}
