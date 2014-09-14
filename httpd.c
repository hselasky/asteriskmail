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

#include <ctype.h>

#include "asteriskmail.h"

static uint32_t base64_bits;
static uint32_t base64_value;

const int
base64_get(const char **pptr)
{
	const char *ptr;
	char ch;

	if (pptr == NULL) {
		base64_bits = 0;
		base64_value = 0;
		return (-1);
	}
	ptr = *pptr;

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
	return (0);
}

const char
base64_get_iso8859_latin1(const char **pptr)
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

void
handle_httpd_connection(int fd)
{
	struct am_message *pamm;
	const char *hdr;
	const char *ptr;
	char buf[4];
	FILE *io;
	int len;
	int num;
	int x;

	io = fdopen(fd, "r+");
	if (io == NULL)
		goto done;

	memset(buf, 0, sizeof(buf));

	/* dump HTTP request header */
	while (1) {
		buf[0] = buf[1];
		buf[1] = buf[2];
		buf[2] = buf[3];
		if (fread(buf + 3, 1, 1, io) != 1)
			goto done;

		if (buf[0] == '\r' && buf[1] == '\n' &&
		    buf[2] == '\r' && buf[3] == '\n')
			break;
	}

	ptr = "HTTP/1.0 200 OK\r\n"
	    "Content-Type: text/html\r\n"
	    "Server: asteriskmail/1.0\r\n"
	    "\r\n"
	    "<html><head><title>AsteriskMail Inbox</title>"
	    "<meta HTTP-EQUIV=\"refresh\" CONTENT=\"120\"></meta>"
	    "</head>"
	    "<h1>List of incoming messages</h1><br>";
	len = strlen(ptr);

	if (fwrite(ptr, 1, len, io) != len)
		goto done;

	pamm = NULL;
	num = 0;
	while (handle_foreach_message(&pamm))
		num++;

	if (num == 0) {
		ptr = "<br><i>There are currently no incoming messages</i><br>";
		len = strlen(ptr);

		if (fwrite(ptr, 1, len, io) != len)
			goto done;
	} else {
		x = 0;
		while (handle_foreach_message(&pamm)) {
			x++;
			fprintf(io, "<h2>Message %d of %d: ", x, num);

			hdr = strstr(pamm->data, "\r\n\r\n");

			ptr = strstr(pamm->data, "Subject: ");
			if (ptr != NULL && ptr < hdr) {
				ptr += 9;
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

			ptr = strstr(pamm->data, "From: ");
			if (ptr != NULL && ptr < hdr) {
				ptr += 6;
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
				fprintf(io, "<meta charset=\"ISO-8859-1\">");

				/* reset parsing */
				base64_get_iso8859_latin1(NULL);

				while (1) {
					int ch;
					uint8_t buf[1];

					ch = base64_get_iso8859_latin1(&ptr);
					if (ch < 0)
						break;
					if (ch != 0 && isprint(ch) != 0) {
						buf[0] = ch;
						if (fwrite(buf, 1, 1, io) != 1)
							goto done;
					}
				}
				fprintf(io, "</meta><br>");
			}
		}
	}
	fprintf(io, "</html>");
	fflush(io);

done:
	if (io != NULL)
		fclose(io);
}
