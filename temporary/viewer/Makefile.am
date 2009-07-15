#
# ---------- header -----------------------------------------------------------
#
# project       infinite
#
# license       GPL
#
# file          /home/mycure/infinit/hole/Makefile.am
#
# created       julien quintard   [wed nov 14 14:38:20 2007]
# updated       julien quintard   [fri mar 21 12:27:57 2008]
#

#
# ---------- flags ------------------------------------------------------------
#

AM_CPPFLAGS		=	-I$(top_srcdir)

#
# ---------- viewer components --------------------------------------------------
#

ubi_DIR		=	ubi

ubi_SRCS		=	$(ubi_DIR)/Viewer.cc
ubi_HDRS		=	$(ubi_DIR)/Viewer.hh

#
# ---------- ubi lib client components --------------------------------------------------
#

ubilib_DIR		=	lib

ubilib_LIBS		=	$(ubilib_DIR)/libubigraphclient.a


#
# ---------- automake ---------------------------------------------------------
#

#
# library
#

lib_LIBRARIES		=	libviewer.a

#
# sources
#

libviewer_a_SOURCES	=	$(ubi_SRCS)

#
# headers
#

ubi_HEADERS		=	$(ubi_HDRS)
ubilib_HEADERS		=	$(ubilib_LIBS)

#
# directories
#

ubidir			=	$(includedir)/viewer/ubi
ubilibdir		=	$(libdir)/

#
# ---------- rules ------------------------------------------------------------
#

CD			=	cd
DOXYGEN			=	doxygen
DOXYFILE		=	documentation/sources/Doxyfile

doxygen:
	(								\
	 $(CD) $(srcdir)						; \
	 $(DOXYGEN) $(DOXYFILE)						\
	)
