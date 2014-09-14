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
handle_smtp_connection(int fd)
{
	struct am_message *pamm = NULL;
	union {
		char	e_mail[ASTERISKMAIL_STRING_MAX];
		char	buffer[128];
	}     u;
	const char *line;
	FILE *io;
	int logged_in = 0;

	io = fdopen(fd, "r+");
	if (io == NULL)
		goto done;

	fprintf(io, "220 %s ESMTP AsteriskMail v1.0\r\n", hostname);
	fflush(io);

	pamm = handle_create_message();
	if (pamm == NULL)
		goto done;

	line = handle_read_line(io);
	if (line == NULL)
		goto done;

	if (handle_compare(line, "HELO ") != 0 &&
	    handle_compare(line, "EHLO ") != 0)
		goto done;

	fprintf(io, "250 Hello %s\r\n", line + 5);

	while (1) {
		fflush(io);
		line = handle_read_line(io);
		if (line == NULL) {
			goto done;
		} else if (handle_compare(line, "MAIL FROM:") == 0) {
			fprintf(io, "250 Ok\r\n");
		} else if (handle_compare(line, "RCPT TO:") == 0) {
			snprintf(u.e_mail, sizeof(u.e_mail), "<localhost@%s>", hostname);
			if (strcmp(line + 8, u.e_mail) == 0) {
				fprintf(io, "250 Ok\r\n");
			} else {
				fprintf(io, "251 User not local\r\n");
			}
		} else if (handle_compare(line, "DATA") == 0) {
			fprintf(io, "354 End data with <CR><LF>.<CR><LF>\r\n");
			fflush(io);
			if (fread(u.buffer, 1, 5, io) != 5)
				goto done;
			while (1) {
				if (u.buffer[0] == '\r' && u.buffer[1] == '\n' &&
				    u.buffer[2] == '.' && u.buffer[3] == '\r' &&
				    u.buffer[4] == '\n')
					break;

				if (u.buffer[0] != 0 &&
				    handle_append_message(pamm, u.buffer[0]) != 0)
					goto done;

				u.buffer[0] = u.buffer[1];
				u.buffer[1] = u.buffer[2];
				u.buffer[2] = u.buffer[3];
				u.buffer[3] = u.buffer[4];
				if (fread(u.buffer + 4, 1, 1, io) != 1)
					goto done;
			}
			/* zero terminate message */
			if (handle_append_message(pamm, 0) != 0)
				goto done;
			handle_insert_message(pamm);
			pamm = NULL;
			fprintf(io, "250 Ok\r\n");
			break;
		} else if (handle_compare(line, "QUIT") == 0) {
			fprintf(io, "221 Bye\r\n");
		} else {
			fprintf(io, "502 Command not implemented\r\n");
		}
	}

	while (1) {
		fflush(io);
		line = handle_read_line(io);
		if (line == NULL) {
			goto done;
		} else if (handle_compare(line, "QUIT") == 0) {
			fprintf(io, "221 Bye\r\n");
		} else {
			fprintf(io, "502 Command not implemented\r\n");
		}
	}

done:
	if (io != NULL)
		fclose(io);
	if (pamm != NULL)
		handle_delete_message(pamm);
}
