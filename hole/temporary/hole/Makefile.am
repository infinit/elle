#
# ---------- header -----------------------------------------------------------
#
# project       infinit
#
# license       infinit (c)
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
# ---------- misc components --------------------------------------------------
#

root_DIR		=	.

root_SRCS		=	$(root_DIR)/Hole.cc		\
				$(root_DIR)/Data.cc		\
				$(root_DIR)/Map_Data.cc		\
				$(root_DIR)/Key.cc

root_HDRS		=	$(root_DIR)/Hole.hh		\
				$(root_DIR)/Data.hh		\
				$(root_DIR)/Map_Data.hh		\
				$(root_DIR)/Key.hh		\
				$(root_DIR)/Key.hxx

#
# ---------- misc components --------------------------------------------------
#

sdht_DIR		=	sdht

sdht_SRCS		=	$(sdht_DIR)/Sdht.cc		\
				$(sdht_DIR)/Sdht_create.cc	\
				$(sdht_DIR)/Sdht_join.cc	\
				$(sdht_DIR)/Sdht_listen.cc	\
				$(sdht_DIR)/Sdht_loop.cc	\
				$(sdht_DIR)/Sdht_ping.cc	\
				$(sdht_DIR)/Sdht_default.cc	\
				$(sdht_DIR)/Sdht_stabilize.cc	\
				$(sdht_DIR)/Sdht_lookup.cc	\
				$(sdht_DIR)/Map_Node.cc		\
				$(sdht_DIR)/Node.cc		\
				$(sdht_DIR)/Map_Request.cc	\
				$(sdht_DIR)/Request.cc

sdht_HDRS		=	$(sdht_DIR)/Sdht.hh		\
				$(sdht_DIR)/Map_Node.hh		\
				$(sdht_DIR)/Node.hh		\
				$(sdht_DIR)/Map_Request.hh	\
				$(sdht_DIR)/Request.hh		\
				$(sdht_DIR)/Request.hxx

#
# ---------- automake ---------------------------------------------------------
#

#
# library
#

lib_LIBRARIES		=	libhole.a

#
# sources
#

libhole_a_SOURCES	=	$(root_SRCS)			\
				$(sdht_SRCS)

#
# headers
#

root_HEADERS		=	$(root_HDRS)
sdht_HEADERS		=	$(sdht_HDRS)

#
# directories
#

rootdir			=	$(includedir)/hole
sdhtdir			=	$(includedir)/hole/sdht

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
