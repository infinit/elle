#
# ---------- header -----------------------------------------------------------
#
# project       infinit
#
# license       GPL
#
# file          /home/mycure/infinit/infinit/Makefile.am
#
# created       julien quintard   [mon feb 16 16:41:39 2009]
# updated       julien quintard   [mon feb 16 19:08:45 2009]
#

#
# ---------- flags ------------------------------------------------------------
#

AM_CPPFLAGS		=	-I$(top_srcdir)

#
# ---------- components -------------------------------------------------------
#

components_DIR		=	components

components_SRCS		=	$(components_DIR)/Address.cc		\
				$(components_DIR)/Block.cc		\
				$(components_DIR)/ContentHashBlock.cc	\
				$(components_DIR)/Directory.cc		\
				$(components_DIR)/Entity.cc		\
				$(components_DIR)/File.cc		\
				$(components_DIR)/PublicKeyBlock.cc	

components_HDRS		=	$(components_DIR)/Components.hh		\
				$(components_DIR)/Address.hh		\
				$(components_DIR)/Block.hh		\
				$(components_DIR)/ContentHashBlock.hh	\
				$(components_DIR)/Directory.hh		\
				$(components_DIR)/Entity.hh		\
				$(components_DIR)/File.hh		\
				$(components_DIR)/PublicKeyBlock.hh	

XXX_SRCS = main.cc

#
# ---------- automake ---------------------------------------------------------
#

#
# executables
#

bin_PROGRAMS		=	infinit

#
# sources
#

infinit_SOURCES		=	$(components_SRCS)			\
				$(components_HDRS)			\
				$(XXX_SRCS)

#
# libraries
#

infinit_LDADD		=	-lelle -lcrypto -lssl -ldl

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
