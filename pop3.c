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

void
handle_pop3_connection(int fd)
{
	const char *line;
	char *username = NULL;
	char *password = NULL;
	FILE *io;
	int logged_in = 0;

	io = fdopen(fd, "r+");
	if (io == NULL)
		goto done;

	fprintf(io, "+OK AsteriskMail v1.0 Ready <%lu@%s>\r\n",
	    random(), hostname);

	while (1) {
		fflush(io);
		line = handle_read_line(io);
		if (line == NULL) {
			goto done;
		} else if (handle_compare(line, "QUIT") == 0) {
			fprintf(io, "+OK\r\n");
			goto done;
		} else if (handle_compare(line, "USER ") == 0) {
			free(username);
			username = strdup(line + 5);
			fprintf(io, "+OK %s selected.\r\n", username);
		} else if (handle_compare(line, "CAPA") == 0) {
			fprintf(io,
			    "+OK List of capabilities follows\r\n"
			    "PLAIN\r\n.\r\n");
		} else if (handle_compare(line, "AUTH PLAIN ") == 0) {
			fprintf(io, "+OK\r\n");
		} else if (handle_compare(line, "PASS ") == 0) {
			free(password);
			password = strdup(line + 5);
			if (username != NULL && password != NULL &&
			    (am_username == NULL || strcmp(username, am_username) == 0) &&
			    (am_password == NULL || strcmp(password, am_password) == 0)) {
				fprintf(io, "+OK Password and username is valid.\r\n");
				break;
			} else {
				fprintf(io, "-ERR Invalid username or password selected.\r\n");
			}
		} else if (handle_compare(line, "WHO") == 0) {
			fprintf(io, "+OK AsteriskMail v1.0\r\n");
		} else if (handle_compare(line, "NOOP") == 0) {
			fprintf(io, "+OK\r\n");
		} else {
			fprintf(io, "-ERR Not logged in yet. Please supply username and password.\r\n");
		}
	}

	while (1) {
		fflush(io);
		line = handle_read_line(io);
		if (line == NULL) {
			goto done;
		} else if (handle_compare(line, "QUIT") == 0) {
			fprintf(io, "+OK\r\n");
			goto done;
		} else if (handle_compare(line, "STAT") == 0) {
			struct am_message *pamm;
			int bytes;
			int num;

			num = 1;
			bytes = 0;
			pamm = NULL;
			while (handle_foreach_message(&pamm)) {
				bytes += pamm->bytes;
				num++;
			}
			fprintf(io, "+OK %d %d\r\n", num - 1, bytes);
		} else if (handle_compare(line, "LIST") == 0) {
			struct am_message *pamm;
			int match;
			int bytes;
			int num;

			if (line[4] != 0)
				match = atoi(line + 5);
			else
				match = -1;

			num = 1;
			bytes = 0;
			pamm = NULL;
			while (handle_foreach_message(&pamm)) {
				bytes += pamm->bytes;
				if (num == match)
					break;
				num++;
			}

			if (match == -1) {
				fprintf(io, "+OK %d messages (%d octets)\r\n", num - 1, bytes);
				num = 1;
				pamm = NULL;
				while (handle_foreach_message(&pamm)) {
					fprintf(io, "%d %d\r\n", num, pamm->bytes);
					num++;
				}
				fprintf(io, ".\r\n");
			} else {
				if (num == match)
					fprintf(io, "+OK %d %d\r\n", match, bytes);
				else
					fprintf(io, "-ERR No such message\r\n");
			}
		} else if (handle_compare(line, "RETR ") == 0) {
			struct am_message *pamm;
			int match;
			int num;

			match = atoi(line + 5);
			num = 1;
			pamm = NULL;
			while (handle_foreach_message(&pamm)) {
				if (num == match) {
					fprintf(io, "+OK %d octets\r\n", pamm->bytes);
					fwrite(pamm->data, 1, pamm->bytes, io);
					fprintf(io, "\r\n.\r\n");
					break;
				}
				num++;
			}
			if (pamm == NULL)
				fprintf(io, "-ERR Non-existing message\r\n");

		} else if (handle_compare(line, "DELE ") == 0) {
			struct am_message *pamm;
			int match;
			int num;

			match = atoi(line + 5);
			num = 1;
			pamm = NULL;
			while (handle_foreach_message(&pamm)) {
				if (num == match) {
					fprintf(io, "+OK message %d deleted\r\n", num);
					handle_delete_message(pamm);
					break;
				}
				num++;
			}
			if (pamm == NULL)
				fprintf(io, "-ERR Non-existing message\r\n");
		} else if (handle_compare(line, "RSET") == 0) {
			fprintf(io, "+OK\r\n");
		} else if (handle_compare(line, "WHO") == 0) {
			fprintf(io, "+OK AsteriskMail v1.0\r\n");
		} else if (handle_compare(line, "NOOP") == 0) {
			fprintf(io, "+OK\r\n");
		} else {
			fprintf(io, "-ERR Invalid command\r\n");
		}
	}
done:
	if (io != NULL)
		fclose(io);
	free(username);
	free(password);
}
