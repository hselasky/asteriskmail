# $FreeBSD: $

BINDIR?= /usr/local/sbin
PROG= asteriskmail
SRCS= asteriskmail.c pop3.c smtp.c httpd.c
MAN=
LDFLAGS= -lutil

.include <bsd.prog.mk>
