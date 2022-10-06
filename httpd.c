/*-
 * Copyright (c) 2014-2022 Hans Petter Selasky. All rights reserved.
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

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#include "asteriskmail.h"

static uint32_t base64_bits;
static uint32_t base64_value;

static int
is_separator(const char ch)
{
	return (ch == '-' || ch == '\t' || ch == '\n' || ch == ' ');
}

const int
base64_get(char **pptr)
{
	char *ptr;
	char ch;

	if (pptr == NULL) {
		base64_bits = 0;
		base64_value = 0;
		return (-1);
	}
	ptr = *pptr;

	while (1) {
		ch = *ptr;
		if (ch == 0)
			return (-1);

		*pptr = ++ptr;

		if (ch >= 'A' && ch <= 'Z')
			ch -= 'A';
		else if (ch >= 'a' && ch <= 'z')
			ch -= 'a' - 26;
		else if (ch >= '0' && ch <= '9')
			ch -= '0' - 52;
		else if (ch == '+')
			ch = 62;
		else if (ch == '/')
			ch = 63;
		else
			return (-1);

		base64_value <<= 6;
		base64_value += (ch & 0x3F);
		base64_bits += 6;

		if (base64_bits >= 8) {
			ch = (base64_value >> (base64_bits - 8));
			base64_bits -= 8;
			return ((uint8_t)ch);
		}
	}
}

const int
base64_get_utf8(char **pptr)
{
	int ch;

	ch = base64_get(pptr);
	if (ch < 0)
		return (ch);
	switch ((uint8_t)ch) {
	case 0x00:
		ch = 0x40;
		break;
	case 0x02:
		ch = 0x24;
		break;
	case 0x11:
		ch = 0x5F;
		break;
	case 0x1B:
		ch = base64_get(pptr);
		switch ((uint8_t)ch) {
		case 0x0A:
			ch = 0x0C;
			break;
		case 0x14:
			ch = 0x5E;
			break;
		case 0x28:
			ch = 0x7B;
			break;
		case 0x29:
			ch = 0x7D;
			break;
		case 0x2F:
			ch = 0x5C;
			break;
		case 0x3C:
			ch = 0x5B;
			break;
		case 0x3D:
			ch = 0x7E;
			break;
		case 0x3E:
			ch = 0x5D;
			break;
		case 0x40:
			ch = 0x7C;
			break;
		default:
			ch = '?';
			break;
		}
		break;
	default:
		break;
	}
	return (ch);
}

static uint8_t
gethex(char ch)
{
	if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (ch - 'a' + 10);
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	return (0);
}

static void
handle_httpd_decode_string(char *ptr)
{
	char *out = ptr;

	while (*ptr) {
		if (ptr[0] == ' ' || ptr[0] == '&' || ptr[0] == '?')
			break;
		if (ptr[0] == '+') {
			*out++ = ' ';
			ptr++;
		} else if (ptr[0] == '%' && ptr[1] != 0 && ptr[2] != 0) {
			*out++ = (gethex(ptr[1]) << 4) | gethex(ptr[2]);
			ptr += 3;
		} else {
			*out++ = *ptr++;
		}
	}
	*out++ = 0;
}

void
handle_httpd_connection(int fd)
{
	enum { MAX_LENGTH = 140 };

	static int curr_sms_id;
	struct am_message *pamm;
	char message_buf[2048];
	char smtpd_buf[2048];
	char system_cmd[512 + 128];
	char *hdr;
	char *ptr;
	char *line;
	char default_phone[64] = {};
	FILE *io;
	int len;
	int num;
	int x;
	int page;
	int y;

	io = fdopen(fd, "r+");
	if (io == NULL)
		goto done;

	page = -1;

	/* dump HTTP request header */
	while (1) {
next_line:
		line = handle_read_line(io);
		if (line == NULL)
			goto done;
		if (line[0] == 0)
			break;
		if (page < 0 && strstr(line, "GET /send_sms.cgi?") == line) {
			char *phone;
			char *message;
			char *id;

			page = 1;

			phone = strstr(line, "&phone=");
			if (phone == NULL)
				phone = strstr(line, "?phone=");

			id = strstr(line, "&id=");
			if (id == NULL)
				id = strstr(line, "?id=");

			message = strstr(line, "&message=");
			if (message == NULL)
				message = strstr(line, "?message=");

			if (phone == NULL || message == NULL || id == NULL) {
				page = 2;
				goto next_line;
			}
			phone += 7;
			message += 9;
			id += 4;

			handle_httpd_decode_string(phone);
			handle_httpd_decode_string(message);
			handle_httpd_decode_string(id);

			ptr = id;
			if (*ptr == 0 || atoi(id) != curr_sms_id) {
				page = 2;
				goto next_line;
			}
			ptr = phone;
			if (*ptr == 0) {
				page = 2;
				goto next_line;
			}
			while (*ptr) {
				if (*ptr == '+') {
					ptr++;
					continue;
				} else if (*ptr >= '0' && *ptr <= '9') {
					ptr++;
					continue;
				}
				page = 2;
				goto next_line;
			}
			ptr = message;
			while (isspace(*ptr)) {
				ptr++;
			}
			if (*ptr == 0) {
				page = 2;
				goto next_line;
			}
			memset(message_buf, ' ', sizeof(message_buf));
			message_buf[sizeof(message_buf) - 1] = 0;

			ptr = message;
			hdr = message_buf;
			while (*ptr) {
				if (*hdr == 0) {
					page = 2;
					goto next_line;
				} else {
					*hdr++ = *ptr++;
				}
			}
			*hdr++ = 0;

			/* make a copy of outgoing messages */
			pamm = handle_create_message();
			if (pamm != NULL) {
				snprintf(smtpd_buf, sizeof(smtpd_buf),
				    "Subject: SMS\r\n"
				    "From: home\r\n"
				    "To: %s <%s>\r\n"
				    "Content-Type: text/html; charset=utf-8\r\n"
				    "\r\n\r\n%s",
				    phone, phone, message);
				ptr = smtpd_buf;
				while (*ptr) {
					if (handle_append_message(pamm, *ptr))
						break;
					ptr++;
				}
				/* zero terminate */
				if (*ptr == 0 &&
				    handle_append_message(pamm, 0) == 0) {
					handle_insert_message(pamm);
				} else {
					handle_delete_message(pamm);
				}
			}

			ptr = message_buf;
			while (1) {
				len = strlen(ptr);
				if (len == 0)
					break;

				/* get maximum length */
				x = len;
				if (x > MAX_LENGTH) {
					x = MAX_LENGTH;

					/* try word separation */
					while (x--) {
						if (is_separator(ptr[x])) {
							while (is_separator(ptr[x]) && x < MAX_LENGTH)
								x++;
							break;
						}
					}

					/* send full text */
					if (x < 1)
						x = MAX_LENGTH;
				}

				y = ptr[x];
				ptr[x] = 0;

				snprintf(system_cmd, sizeof(system_cmd),
				    "/usr/local/sbin/asterisk"
				    " -rx \"dongle sms dongle0 %s "
				    "\\\"%s\\\"\"", phone, ptr);

				if (system(system_cmd) != 0) {
					page = 3;
					goto next_line;
				}

				/* nice operation a bit */
				usleep(250000);

				ptr[x] = y;
				ptr += x;
			}
			curr_sms_id++;
			if (curr_sms_id >= 10000)
				curr_sms_id = 0;

		} else if (page < 0 && (strstr(line, "GET / ") == line ||
		    strstr(line, "GET /index.html") == line)) {
			page = 4;
		} else if (page < 0 && strstr(line, "GET /sms_form.html") == line) {
			char *phone;

			phone = strstr(line, "&phone=");
			if (phone == NULL)
				phone = strstr(line, "?phone=");
			if (phone != NULL) {
				phone += 7;
				handle_httpd_decode_string(phone);
				strlcpy(default_phone, phone, sizeof(default_phone));
			}

			page = 5;
		}
	}

	switch (page) {
	case 1:
		fprintf(io, "HTTP/1.0 200 OK\r\n"
		    "Content-Type: text/html\r\n"
		    "Server: asteriskmail/1.0\r\n"
		    "\r\n"
		    "<html><head><title>AsteriskMail Inbox</title>"
		    "</head>"
		    "<h1>SMS was successfully sent. <a HREF=\"index.html\">Click here to go back</a>.</h1><br>"
		    "</html>");
		goto done;
	case 2:
		fprintf(io, "HTTP/1.0 200 OK\r\n"
		    "Content-Type: text/html\r\n"
		    "Server: asteriskmail/1.0\r\n"
		    "\r\n"
		    "<html><head><title>AsteriskMail Inbox</title>"
		    "</head>"
		    "<h1>ERROR: Invalid SMS message, phone number or ID.<br><a HREF=\"sms_form.html\">Click here to retry</a></h1><br>"
		    "</html>");
		goto done;
	case 3:
		fprintf(io, "HTTP/1.0 200 OK\r\n"
		    "Content-Type: text/html\r\n"
		    "Server: asteriskmail/1.0\r\n"
		    "\r\n"
		    "<html><head><title>AsteriskMail Inbox</title>"
		    "</head>"
		    "<h1>ERROR: Sending SMS.<br><a HREF=\"sms_form.html\">Click here to retry</a></h1><br>"
		    "</html>");
		goto done;
	case 5:
		fprintf(io, "HTTP/1.0 200 OK\r\n"
		    "Content-Type: text/html\r\n"
		    "Server: asteriskmail/1.0\r\n"
		    "\r\n"
		    "<html><head><title>AsteriskMail Send SMS</title>"
		    "</head>"
		    "<br><br><form action=\"send_sms.cgi\" id=\"smsform\" accept-charset=\"UTF-8\">"
		    "<table bgcolor=\"#c0c0c0\">"
		    "<tr><th COLSPAN=\"2\">Send SMS</th></tr>"
		    "<tr><th>"
		    "<div align=\"right\">Mobile:</div></th><th><div align=\"left\">"
		    "<input type=\"tel\" maxlength=\"30\" name=\"phone\" value=\"%s\"></div></th></tr>"
		    "<tr><th>"
		    "<div align=\"right\">Message:</div></th><th><div align=\"left\">"
		    "<textarea maxlength=\"%d\" name=\"message\" form=\"smsform\" autocomplete=\"off\" "
		    "wrap=\"logical\" rows=\"12\" cols=\"32\" type=\"password\">"
		    "</textarea></div></th></tr>"
		    "<tr><th></th><th>"
		    "<div align=\"right\"><input type=\"submit\" value=\"Submit\"></div>"
		    "</th></table>"
		    "<input type=\"hidden\" name=\"id\" value=\"%d\"> "
		    "</form>"
		    "<br><a HREF=\"index.html\">Click here to go back</a>"
		    "</html>", default_phone, MAX_LENGTH * 10, curr_sms_id);
		goto done;
	case 4:
		break;
	default:
		fprintf(io, "HTTP/1.0 200 OK\r\n"
		    "Content-Type: text/html\r\n"
		    "Server: asteriskmail/1.0\r\n"
		    "\r\n"
		    "<html><head><title>AsteriskMail Inbox</title>"
		    "</head>"
		    "<h1>Invalid page requested! <a HREF=\"index.html\">Click here to go back</a>.</h1><br>"
		    "</html>");
		goto done;
	}

	fprintf(io, "HTTP/1.0 200 OK\r\n"
	    "Content-Type: text/html\r\n"
	    "Server: asteriskmail/1.0\r\n"
	    "\r\n"
	    "<html><head><title>AsteriskMail Inbox</title>"
	    "<meta HTTP-EQUIV=\"refresh\" CONTENT=\"120\">"
	    "<meta charset=\"UTF-8\">"
	    "</meta>"
	    "</head>"
	    "<h1>List of incoming messages</h1><br>");

	pamm = NULL;
	num = 0;
	while (handle_foreach_message(&pamm))
		num++;

	if (num == 0) {
		fprintf(io, "<br><i>There are currently no incoming messages</i><br>");
	} else {
		x = 0;
		while (handle_foreach_message(&pamm)) {
			x++;
			fprintf(io, "<h2>Message %d of %d: ", x, num);

			hdr = strstr(pamm->data, "\r\n\r\n");
			if (hdr == NULL)
				hdr = pamm->data + strlen(pamm->data);
			else
				hdr += 4;

			ptr = strafter(pamm->data, "\r\nSubject: ");
			if (ptr == NULL)
				ptr = strafter(pamm->data, "\nSubject: ");
			if (ptr != NULL && ptr <= hdr) {
				while (1) {
					char ch;

					ch = *ptr++;
					if (isprint(ch) == 0)
						break;
					if (fwrite(&ch, 1, 1, io) != 1)
						goto done;
				}
			}
			fprintf(io, " - ");

			ptr = strafter(pamm->data, "\r\nFrom: ");
			if (ptr == NULL)
				ptr = strafter(pamm->data, "\nFrom: ");
			if (ptr != NULL && ptr <= hdr) {
				bool done = false;
				uint8_t offset = 0;
				char telno[64];

				while (1) {
					char ch;

					ch = *ptr++;
					if (isprint(ch) == 0)
						break;
					if (isdigit(ch) && done == false && offset < (uint8_t)(sizeof(telno) - 1)) {
						telno[offset++] = ch;
					} else if (ch == '+' && done == false && offset < (uint8_t)(sizeof(telno) - 3)) {
						telno[offset++] = '%';
						telno[offset++] = '2';
						telno[offset++] = 'b';
					} else {
						if (offset != 0)
							done = true;
					}
					if (fwrite(&ch, 1, 1, io) != 1)
						goto done;
				}
				telno[offset] = 0;

				if (offset != 0)
					fprintf(io, " - <a href=\"/sms_form.html?phone=%s\">reply</a></h2><br>", telno);
				else
					fprintf(io, "</h2><br>");
			} else {
				fprintf(io, "</h2><br>");
			}

			ptr = hdr;
			if (ptr != NULL && ptr[0] != 0) {
				while (*ptr != 0) {
					if (*ptr == '<') {
						if (fwrite("&lt;", 1, 4, io) != 4)
							goto done;
					} else if (*ptr == '>') {
						if (fwrite("&gt;", 1, 4, io) != 4)
							goto done;
					} else if (*ptr == '\"') {
						if (fwrite("&quot;", 1, 6, io) != 6)
							goto done;
					} else {
						if (fwrite(ptr, 1, 1, io) != 1)
							goto done;
					}
					ptr++;
				}
				fprintf(io, "<br>");
			}
		}
	}

	fprintf(io,
	    "<br><a HREF=\"sms_form.html\">Click here to send SMS</a>"
	    "</html>");
done:
	if (io != NULL) {
		fflush(io);
		fclose(io);
	}
}
