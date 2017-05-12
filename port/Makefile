# Created by: Stephen Hurd <shurd@FreeBSD.org>
# $FreeBSD: head/graphics/svgalib/Makefile 357654 2014-06-13 07:36:27Z olgeni $

PORTNAME=	svgalib
PORTVERSION=	1.9.25
CATEGORIES=	graphics devel
MASTER_SITES=	http://mail.arava.co.il/~matan/svgalib/
PKGNAMESUFFIX=	-devel

MAINTAINER=	shurd@FreeBSD.org
COMMENT=	Low level console graphics library

ONLY_FOR_ARCHS=	i386 amd64

USES=	gmake uidfix dos2unix shebangfix
DOS2UNIX_FILES=	utils/gtf/gtf.h utils/gtf/scitech.h
SHEBANG_FILES=	utils/svgakeymap
USE_LDCONFIG=	yes

ALL_TARGET=	shared static
MAKE_ENV=	NO_HELPER=y

.include <bsd.port.pre.mk>

.if ${ARCH} != "i386"
MAKE_ENV+=	NO_ASM=y
.endif

.include <bsd.port.post.mk>
