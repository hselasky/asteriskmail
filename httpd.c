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

#include <stdlib.h>
#include <ctype.h>

#include "asteriskmail.h"

static uint32_t base64_bits;
static uint32_t base64_value;

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
base64_get_iso8859_latin1(char **pptr)
{
	int ch;

	ch = base64_get(pptr);
	if (ch < 0)
		return (ch);
	switch ((uint8_t)ch) {
	case 0x00:
		ch = 0x40;
		break;
	case 0x01:
		ch = 0xA3;
		break;
	case 0x02:
		ch = 0x24;
		break;
	case 0x03:
		ch = 0xA5;
		break;
	case 0x04:
		ch = 0xE8;
		break;
	case 0x05:
		ch = 0xE9;
		break;
	case 0x06:
		ch = 0xF9;
		break;
	case 0x07:
		ch = 0xEC;
		break;
	case 0x08:
		ch = 0xF2;
		break;
	case 0x09:
		ch = 0xC7;
		break;
	case 0x0A:
		ch = 0x0A;
		break;
	case 0x0B:
		ch = 0xD8;
		break;
	case 0x0C:
		ch = 0xF8;
		break;
	case 0x0D:
		ch = 0x0D;
		break;
	case 0x0E:
		ch = 0xC5;
		break;
	case 0x0F:
		ch = 0xE5;
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
	case 0x1C:
		ch = 0xC6;
		break;
	case 0x1D:
		ch = 0xE6;
		break;
	case 0x1E:
		ch = 0xDF;
		break;
	case 0x1F:
		ch = 0xC9;
		break;
	case 0x20:
		ch = 0x20;
		break;
	case 0x21:
		ch = 0x21;
		break;
	case 0x22:
		ch = 0x22;
		break;
	case 0x23:
		ch = 0x23;
		break;
	case 0x24:
		ch = 0xA4;
		break;
	case 0x25:
		ch = 0x25;
		break;
	case 0x26:
		ch = 0x26;
		break;
	case 0x27:
		ch = 0x27;
		break;
	case 0x28:
		ch = 0x28;
		break;
	case 0x29:
		ch = 0x29;
		break;
	case 0x2A:
		ch = 0x2A;
		break;
	case 0x2B:
		ch = 0x2B;
		break;
	case 0x2C:
		ch = 0x2C;
		break;
	case 0x2D:
		ch = 0x2D;
		break;
	case 0x2E:
		ch = 0x2E;
		break;
	case 0x2F:
		ch = 0x2F;
		break;
	case 0x30:
		ch = 0x30;
		break;
	case 0x31:
		ch = 0x31;
		break;
	case 0x32:
		ch = 0x32;
		break;
	case 0x33:
		ch = 0x33;
		break;
	case 0x34:
		ch = 0x34;
		break;
	case 0x35:
		ch = 0x35;
		break;
	case 0x36:
		ch = 0x36;
		break;
	case 0x37:
		ch = 0x37;
		break;
	case 0x38:
		ch = 0x38;
		break;
	case 0x39:
		ch = 0x39;
		break;
	case 0x3A:
		ch = 0x3A;
		break;
	case 0x3B:
		ch = 0x3B;
		break;
	case 0x3C:
		ch = 0x3C;
		break;
	case 0x3D:
		ch = 0x3D;
		break;
	case 0x3E:
		ch = 0x3E;
		break;
	case 0x3F:
		ch = 0x3F;
		break;
	case 0x40:
		ch = 0xA1;
		break;
	case 0x41:
		ch = 0x41;
		break;
	case 0x42:
		ch = 0x42;
		break;
	case 0x43:
		ch = 0x43;
		break;
	case 0x44:
		ch = 0x44;
		break;
	case 0x45:
		ch = 0x45;
		break;
	case 0x46:
		ch = 0x46;
		break;
	case 0x47:
		ch = 0x47;
		break;
	case 0x48:
		ch = 0x48;
		break;
	case 0x49:
		ch = 0x49;
		break;
	case 0x4A:
		ch = 0x4A;
		break;
	case 0x4B:
		ch = 0x4B;
		break;
	case 0x4C:
		ch = 0x4C;
		break;
	case 0x4D:
		ch = 0x4D;
		break;
	case 0x4E:
		ch = 0x4E;
		break;
	case 0x4F:
		ch = 0x4F;
		break;
	case 0x50:
		ch = 0x50;
		break;
	case 0x51:
		ch = 0x51;
		break;
	case 0x52:
		ch = 0x52;
		break;
	case 0x53:
		ch = 0x53;
		break;
	case 0x54:
		ch = 0x54;
		break;
	case 0x55:
		ch = 0x55;
		break;
	case 0x56:
		ch = 0x56;
		break;
	case 0x57:
		ch = 0x57;
		break;
	case 0x58:
		ch = 0x58;
		break;
	case 0x59:
		ch = 0x59;
		break;
	case 0x5A:
		ch = 0x5A;
		break;
	case 0x5B:
		ch = 0xC4;
		break;
	case 0x5C:
		ch = 0xD6;
		break;
	case 0x5D:
		ch = 0xD1;
		break;
	case 0x5E:
		ch = 0xDC;
		break;
	case 0x5F:
		ch = 0xA7;
		break;
	case 0x60:
		ch = 0xBF;
		break;
	case 0x61:
		ch = 0x61;
		break;
	case 0x62:
		ch = 0x62;
		break;
	case 0x63:
		ch = 0x63;
		break;
	case 0x64:
		ch = 0x64;
		break;
	case 0x65:
		ch = 0x65;
		break;
	case 0x66:
		ch = 0x66;
		break;
	case 0x67:
		ch = 0x67;
		break;
	case 0x68:
		ch = 0x68;
		break;
	case 0x69:
		ch = 0x69;
		break;
	case 0x6A:
		ch = 0x6A;
		break;
	case 0x6B:
		ch = 0x6B;
		break;
	case 0x6C:
		ch = 0x6C;
		break;
	case 0x6D:
		ch = 0x6D;
		break;
	case 0x6E:
		ch = 0x6E;
		break;
	case 0x6F:
		ch = 0x6F;
		break;
	case 0x70:
		ch = 0x70;
		break;
	case 0x71:
		ch = 0x71;
		break;
	case 0x72:
		ch = 0x72;
		break;
	case 0x73:
		ch = 0x73;
		break;
	case 0x74:
		ch = 0x74;
		break;
	case 0x75:
		ch = 0x75;
		break;
	case 0x76:
		ch = 0x76;
		break;
	case 0x77:
		ch = 0x77;
		break;
	case 0x78:
		ch = 0x78;
		break;
	case 0x79:
		ch = 0x79;
		break;
	case 0x7A:
		ch = 0x7A;
		break;
	case 0x7B:
		ch = 0xE4;
		break;
	case 0x7C:
		ch = 0xF6;
		break;
	case 0x7D:
		ch = 0xF1;
		break;
	case 0x7E:
		ch = 0xFC;
		break;
	case 0x7F:
		ch = 0xE0;
		break;
	default:
		ch = '?';
		break;
	}
	return ((uint8_t)ch);
}

int
iso8859_to_sms(char **pptr, char ch)
{
	char *ptr = *pptr;

	if (ptr[0] == 0 || ptr[1] == 0)
		return (-1);

	switch ((uint8_t)ch) {
	case 0x40:
		ch = 0x00;
		break;
	case 0xA3:
		ch = 0x01;
		break;
	case 0x24:
		ch = 0x02;
		break;
	case 0xA5:
		ch = 0x03;
		break;
	case 0xE8:
		ch = 0x04;
		break;
	case 0xE9:
		ch = 0x05;
		break;
	case 0xF9:
		ch = 0x06;
		break;
	case 0xEC:
		ch = 0x07;
		break;
	case 0xF2:
		ch = 0x08;
		break;
	case 0xC7:
		ch = 0x09;
		break;
	case 0x0A:
		ch = 0x0A;
		break;
	case 0xD8:
		ch = 0x0B;
		break;
	case 0xF8:
		ch = 0x0C;
		break;
	case 0x0D:
		ch = 0x0D;
		break;
	case 0xC5:
		ch = 0x0E;
		break;
	case 0xE5:
		ch = 0x0F;
		break;
	case 0x5F:
		ch = 0x11;
		break;
	case 0x0C:
		*ptr++ = 0x1B;
		ch = 0x0A;
		break;
	case 0x5E:
		*ptr++ = 0x1B;
		ch = 0x14;
		break;
	case 0x7B:
		*ptr++ = 0x1B;
		ch = 0x28;
		break;
	case 0x7D:
		*ptr++ = 0x1B;
		ch = 0x29;
		break;
	case 0x5C:
		*ptr++ = 0x1B;
		ch = 0x2F;
		break;
	case 0x5B:
		*ptr++ = 0x1B;
		ch = 0x3C;
		break;
	case 0x7E:
		*ptr++ = 0x1B;
		ch = 0x3D;
		break;
	case 0x5D:
		*ptr++ = 0x1B;
		ch = 0x3E;
		break;
	case 0x7C:
		*ptr++ = 0x1B;
		ch = 0x40;
		break;
		break;
	case 0xC6:
		ch = 0x1C;
		break;
	case 0xE6:
		ch = 0x1D;
		break;
	case 0xDF:
		ch = 0x1E;
		break;
	case 0xC9:
		ch = 0x1F;
		break;
	case 0x20:
		ch = 0x20;
		break;
	case 0x21:
		ch = 0x21;
		break;
	case 0x22:
		/* XXX don't forward quotes */
		/* ch = 0x22; */
		ch = 0x27;
		break;
	case 0x23:
		ch = 0x23;
		break;
	case 0xA4:
		ch = 0x24;
		break;
	case 0x25:
		ch = 0x25;
		break;
	case 0x26:
		ch = 0x26;
		break;
	case 0x27:
		ch = 0x27;
		break;
	case 0x28:
		ch = 0x28;
		break;
	case 0x29:
		ch = 0x29;
		break;
	case 0x2A:
		ch = 0x2A;
		break;
	case 0x2B:
		ch = 0x2B;
		break;
	case 0x2C:
		ch = 0x2C;
		break;
	case 0x2D:
		ch = 0x2D;
		break;
	case 0x2E:
		ch = 0x2E;
		break;
	case 0x2F:
		ch = 0x2F;
		break;
	case 0x30:
		ch = 0x30;
		break;
	case 0x31:
		ch = 0x31;
		break;
	case 0x32:
		ch = 0x32;
		break;
	case 0x33:
		ch = 0x33;
		break;
	case 0x34:
		ch = 0x34;
		break;
	case 0x35:
		ch = 0x35;
		break;
	case 0x36:
		ch = 0x36;
		break;
	case 0x37:
		ch = 0x37;
		break;
	case 0x38:
		ch = 0x38;
		break;
	case 0x39:
		ch = 0x39;
		break;
	case 0x3A:
		ch = 0x3A;
		break;
	case 0x3B:
		ch = 0x3B;
		break;
	case 0x3C:
		ch = 0x3C;
		break;
	case 0x3D:
		ch = 0x3D;
		break;
	case 0x3E:
		ch = 0x3E;
		break;
	case 0x3F:
		ch = 0x3F;
		break;
	case 0xA1:
		ch = 0x40;
		break;
	case 0x41:
		ch = 0x41;
		break;
	case 0x42:
		ch = 0x42;
		break;
	case 0x43:
		ch = 0x43;
		break;
	case 0x44:
		ch = 0x44;
		break;
	case 0x45:
		ch = 0x45;
		break;
	case 0x46:
		ch = 0x46;
		break;
	case 0x47:
		ch = 0x47;
		break;
	case 0x48:
		ch = 0x48;
		break;
	case 0x49:
		ch = 0x49;
		break;
	case 0x4A:
		ch = 0x4A;
		break;
	case 0x4B:
		ch = 0x4B;
		break;
	case 0x4C:
		ch = 0x4C;
		break;
	case 0x4D:
		ch = 0x4D;
		break;
	case 0x4E:
		ch = 0x4E;
		break;
	case 0x4F:
		ch = 0x4F;
		break;
	case 0x50:
		ch = 0x50;
		break;
	case 0x51:
		ch = 0x51;
		break;
	case 0x52:
		ch = 0x52;
		break;
	case 0x53:
		ch = 0x53;
		break;
	case 0x54:
		ch = 0x54;
		break;
	case 0x55:
		ch = 0x55;
		break;
	case 0x56:
		ch = 0x56;
		break;
	case 0x57:
		ch = 0x57;
		break;
	case 0x58:
		ch = 0x58;
		break;
	case 0x59:
		ch = 0x59;
		break;
	case 0x5A:
		ch = 0x5A;
		break;
	case 0xC4:
		ch = 0x5B;
		break;
	case 0xD6:
		ch = 0x5C;
		break;
	case 0xD1:
		ch = 0x5D;
		break;
	case 0xDC:
		ch = 0x5E;
		break;
	case 0xA7:
		ch = 0x5F;
		break;
	case 0xBF:
		ch = 0x60;
		break;
	case 0x61:
		ch = 0x61;
		break;
	case 0x62:
		ch = 0x62;
		break;
	case 0x63:
		ch = 0x63;
		break;
	case 0x64:
		ch = 0x64;
		break;
	case 0x65:
		ch = 0x65;
		break;
	case 0x66:
		ch = 0x66;
		break;
	case 0x67:
		ch = 0x67;
		break;
	case 0x68:
		ch = 0x68;
		break;
	case 0x69:
		ch = 0x69;
		break;
	case 0x6A:
		ch = 0x6A;
		break;
	case 0x6B:
		ch = 0x6B;
		break;
	case 0x6C:
		ch = 0x6C;
		break;
	case 0x6D:
		ch = 0x6D;
		break;
	case 0x6E:
		ch = 0x6E;
		break;
	case 0x6F:
		ch = 0x6F;
		break;
	case 0x70:
		ch = 0x70;
		break;
	case 0x71:
		ch = 0x71;
		break;
	case 0x72:
		ch = 0x72;
		break;
	case 0x73:
		ch = 0x73;
		break;
	case 0x74:
		ch = 0x74;
		break;
	case 0x75:
		ch = 0x75;
		break;
	case 0x76:
		ch = 0x76;
		break;
	case 0x77:
		ch = 0x77;
		break;
	case 0x78:
		ch = 0x78;
		break;
	case 0x79:
		ch = 0x79;
		break;
	case 0x7A:
		ch = 0x7A;
		break;
	case 0xE4:
		ch = 0x7B;
		break;
	case 0xF6:
		ch = 0x7C;
		break;
	case 0xF1:
		ch = 0x7D;
		break;
	case 0xFC:
		ch = 0x7E;
		break;
	case 0xE0:
		ch = 0x7F;
		break;
	default:
		ch = 0x3F;
		break;
	}
	*ptr++ = ch;
	*pptr = ptr;
	return (0);
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
	struct am_message *pamm;
	char message_buf[512];
	char system_cmd[512 + 128];
	char *hdr;
	char *ptr;
	char *line;
	FILE *io;
	int len;
	int num;
	int x;
	int page;

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

			page = 1;

			phone = strstr(line, "&phone=");
			if (phone == NULL)
				phone = strstr(line, "?phone=");

			message = strstr(line, "&message=");
			if (message == NULL)
				message = strstr(line, "?message=");

			if (phone == NULL || message == NULL) {
				page = 2;
				goto next_line;
			}
			phone += 7;
			message += 9;

			handle_httpd_decode_string(phone);
			handle_httpd_decode_string(message);

			printf("%s %s\n", phone, message);

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
				if (iso8859_to_sms(&hdr, *ptr++) < 0) {
					page = 2;
					goto next_line;
				}
			}
			*hdr++ = 0;

			snprintf(system_cmd, sizeof(system_cmd),
			    "asterisk -rx \"dongle sms dongle0 %s \\\"%s\\\"\"", phone, message_buf);
			if (system(system_cmd) != 0) {
				page = 3;
				goto next_line;
			}
			/* nice operation a bit */
			usleep(1000000);
		} else if (page < 0 && (strstr(line, "GET / ") == line ||
		    strstr(line, "GET /index.html") == line)) {
			page = 4;
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
		    "<h1>Invalid SMS message or phone number. Cannot send SMS!</h1><br>"
		    "</html>");
		goto done;
	case 3:
		fprintf(io, "HTTP/1.0 200 OK\r\n"
		    "Content-Type: text/html\r\n"
		    "Server: asteriskmail/1.0\r\n"
		    "\r\n"
		    "<html><head><title>AsteriskMail Inbox</title>"
		    "</head>"
		    "<h1>Error sending SMS!</h1><br>"
		    "</html>");
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
	    "<meta HTTP-EQUIV=\"refresh\" CONTENT=\"120\"></meta>"
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
				while (1) {
					char ch;

					ch = *ptr++;
					if (isprint(ch) == 0)
						break;
					if (fwrite(&ch, 1, 1, io) != 1)
						goto done;
				}
			}
			fprintf(io, "</h2><br>");

			ptr = hdr;
			if (ptr != NULL) {
				ptr += 4;
				len = strlen(ptr);

				fprintf(io, "<meta charset=\"ISO-8859-1\">");

				if (fwrite(ptr, 1, len, io) != len)
					goto done;

				fprintf(io, "</meta><br>");
			}
		}
	}

	fprintf(io,
	    "<br><br><form action=\"send_sms.cgi\" id=\"smsform\" accept-charset=\"ISO-8859-1\">"
	    "<table bgcolor=\"#c0c0c0\">"
	    "<tr><th COLSPAN=\"2\">Send SMS</th></tr>"
	    "<tr><th>"
	    "<div align=\"right\">Mobile:</div></th><th><div align=\"left\">"
	    "<input type=\"tel\" maxlength=\"30\" name=\"phone\"></div></th></tr>"
	    "<tr><th>"
	    "<div align=\"right\">Message:</div></th><th><div align=\"left\">"
	    "<textarea maxlength=\"130\" name=\"message\" form=\"smsform\" autocomplete=\"off\" "
	    "wrap=\"logical\" rows=\"12\" cols=\"32\">"
	    "</textarea></div></th></tr>"
	    "<tr><th></th><th>"
	    "<div align=\"right\"><input type=\"submit\" value=\"Submit\"></div>"
	    "</th></table>"
	    "</form>"
	    "</html>");
done:
	if (io != NULL) {
		fflush(io);
		fclose(io);
	}
}
