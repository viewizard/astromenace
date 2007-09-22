# New ports collection makefile for: pro-search
# Date created:		23 Sep 2007
# Whom:			Oleg Alexeenkov
#
# $FreeBSD:$
#

PORTNAME=	openastromenace
PORTDIRNAME=	openastromenace
PORTVERSION=	1.2.0
CATEGORIES=	games
MASTER_SITES=	${MASTER_SITE_SOURCEFORGE}
MASTER_SITE_SUBDIR=    ${PORTNAME}
#DIST_SUBDIR=    search

MAINTAINER=	proler@gmail.com
COMMENT=	Game.

USE_BZIP2=	on
WANT_SDL=       yes
USE_SDL=        sdl

CMAKE_ARGS=     -DCMAKE_BUILD_TYPE:STRING=Release \
                -DCMAKE_C_COMPILER:STRING='${CC}' \
                -DCMAKE_CXX_COMPILER:STRING=${CXX} \
                -DCMAKE_CXX_FLAGS:STRING="${CXXFLAGS}" \
                -DCMAKE_INSTALL_PREFIX=${PREFIX}


BUILD_DEPENDS+=\
    ${LOCALBASE}/bin/sdl-config:${PORTSDIR}/devel/sdl12	\
    ${LOCALBASE}/bin/openal-config:${PORTSDIR}/audio/openal	\
    ${LOCALBASE}/bin/cmake:${PORTSDIR}/devel/cmake	\

LIB_DEPENDS+=   vorbisfile.5:${PORTSDIR}/audio/libvorbis	\
		jpeg.9:${PORTSDIR}/graphics/jpeg 


#CONFIGURE_SCRIPT=	cmake
#CONFIGURE_ARGS=	.


OPTIONS=        \
		SRC        	"src (!not ready)"	off	\
		DATA        	"data file "	on	\
		EN        	" en lang"	on	\
		DE        	" de lang "	off	\
		RU        	" ru lang "	off	\


.include <bsd.port.pre.mk>

.undef DISTFILES
.	if defined(WITH_SRC)
        DISTFILES+=oamenace-src-${PORTVERSION}.tar.bz2
.	endif
.	if defined(WITH_DATA)
        DISTFILES+=oamenace-data-${PORTVERSION}.tar.bz2
.	endif
.	if defined(WITH_EN)
        DISTFILES+=oamenace-lang-en-${PORTVERSION}.tar.bz2
.	endif
.	if defined(WITH_DE)
        DISTFILES+=oamenace-lang-de-${PORTVERSION}.tar.bz2
.	endif
.	if defined(WITH_RU)
        DISTFILES+=oamenace-lang-ru-${PORTVERSION}.tar.bz2
.	endif


#RUN_DEPENDS=	${BUILD_DEPENDS}

#WRKSRC=         ${WRKDIR}/${DIST_SUBDIR}

#do-install:


do-configure:
	@(cd ${WRKSRC}; \
	${LOCALBASE}/bin/cmake ${CMAKE_ARGS} ${PATCH_WRKSRC})


do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/AstroMenace ${PREFIX}/bin
#post-install:
	mkdir -p ${DATADIR}
.if defined(WITH_DATA)
	${INSTALL_DATA} ${WRKDIR}/gamedata.vfs ${DATADIR}/
.endif
.if defined(WITH_EN)
	${INSTALL_DATA} ${WRKDIR}/gamelang_en.vfs ${DATADIR}/
.endif
.if defined(WITH_DE)
	${INSTALL_DATA} ${WRKDIR}/gamelang_de.vfs ${DATADIR}/
.endif
.if defined(WITH_RU)
	${INSTALL_DATA} ${WRKDIR}/gamelang_ru.vfs ${DATADIR}/
.endif


.include <bsd.port.post.mk>
#.include <bsd.port.mk>
