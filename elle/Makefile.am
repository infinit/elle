#
# ---------- header -----------------------------------------------------------
#
# project       infinit
#
# license       infinit (c)
#
# file          /home/mycure/infinit/elle/Makefile.am
#
# created       julien quintard   [wed nov 14 14:38:20 2007]
# updated       julien quintard   [mon feb 16 19:12:25 2009]
#

#
# ---------- flags ------------------------------------------------------------
#

AM_CPPFLAGS		=	-I$(top_srcdir)

#
# ---------- elle -------------------------------------------------------------
#

elle_HDRS		=	Elle.hh

#
# ---------- core -------------------------------------------------------------
#

core_DIR		=	core

core_SRCS		=	$(core_DIR)/Large.cc			\
				$(core_DIR)/Null.cc			

core_HDRS		=	$(core_DIR)/Boolean.hh			\
				$(core_DIR)/Character.hh		\
				$(core_DIR)/Core.hh			\
				$(core_DIR)/Core.hxx			\
				$(core_DIR)/Integer.hh			\
				$(core_DIR)/Large.hh			\
				$(core_DIR)/Natural.hh			\
				$(core_DIR)/Null.hh			\
				$(core_DIR)/Object.hh			\
				$(core_DIR)/Real.hh			\
				$(core_DIR)/String.hh			

#
# ---------- misc -------------------------------------------------------------
#

misc_DIR		=	misc

misc_SRCS		=	$(misc_DIR)/Region.cc			\
				$(misc_DIR)/Report.cc			

misc_HDRS		=	$(misc_DIR)/Byte.hh			\
				$(misc_DIR)/Misc.hh			\
				$(misc_DIR)/Region.hh			\
				$(misc_DIR)/Report.hh			\
				$(misc_DIR)/Status.hh			

#
# ---------- io ---------------------------------------------------------------
#

io_DIR			=	io

io_SRCS			=	$(io_DIR)/IO.cc				

io_HDRS			=	$(io_DIR)/Dumpable.hh			\
				$(io_DIR)/IO.hh				

#
# ---------- archive ----------------------------------------------------------
#

archive_DIR		=	archive

archive_SRCS		=	$(archive_DIR)/Archive.cc		

archive_HDRS		=	$(archive_DIR)/Archive.hh		\
				$(archive_DIR)/Archive.hxx		\
				$(archive_DIR)/Archivable.hh		

#
# ---------- system -----------------------------------------------------------
#

system_DIR		=	system

system_SRCS		=	$(system_DIR)/System.cc		

system_HDRS		=	$(system_DIR)/System.hh			

#
# ---------- crypto -----------------------------------------------------------
#

crypto_DIR		=	crypto

crypto_SRCS		=	$(crypto_DIR)/Cipher.cc			\
				$(crypto_DIR)/Code.cc			\
				$(crypto_DIR)/Cryptography.cc		\
				$(crypto_DIR)/Digest.cc			\
				$(crypto_DIR)/KeyPair.cc		\
				$(crypto_DIR)/OneWay.cc			\
				$(crypto_DIR)/PrivateKey.cc		\
				$(crypto_DIR)/PublicKey.cc		\
				$(crypto_DIR)/SecretKey.cc		\
				$(crypto_DIR)/Signature.cc		

crypto_HDRS		=	$(crypto_DIR)/Cipher.hh			\
				$(crypto_DIR)/Clear.hh			\
				$(crypto_DIR)/Code.hh			\
				$(crypto_DIR)/Cryptography.hh		\
				$(crypto_DIR)/Digest.hh			\
				$(crypto_DIR)/KeyPair.hh		\
				$(crypto_DIR)/OneWay.hh			\
				$(crypto_DIR)/Plain.hh			\
				$(crypto_DIR)/PrivateKey.hh		\
				$(crypto_DIR)/PublicKey.hh		\
				$(crypto_DIR)/SecretKey.hh		\
				$(crypto_DIR)/Signature.hh		

#
# ---------- automake ---------------------------------------------------------
#

#
# library
#

lib_LIBRARIES		=	libelle.a

#
# sources
#

libelle_a_SOURCES	=	$(core_SRCS)				\
				$(misc_SRCS)				\
				$(io_SRCS)				\
				$(archive_SRCS)				\
				$(system_SRCS)				\
				$(crypto_SRCS)				

#
# headers
#

elle_HEADERS		=	$(elle_HDRS)
core_HEADERS		=	$(core_HDRS)
misc_HEADERS		=	$(misc_HDRS)
io_HEADERS		=	$(io_HDRS)
archive_HEADERS		=	$(archive_HDRS)
system_HEADERS		=	$(system_HDRS)
crypto_HEADERS		=	$(crypto_HDRS)

#
# directories
#

elledir			=	$(includedir)/
coredir			=	$(includedir)/elle/core
miscdir			=	$(includedir)/elle/misc
iodir			=	$(includedir)/elle/io
cryptodir		=	$(includedir)/elle/crypto
archivedir		=	$(includedir)/elle/archive
systemdir		=	$(includedir)/elle/system

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
