#
# ---------- header -----------------------------------------------------------
#
# project       infinit
#
# license       infinit
#
# author        julien quintard   [wed oct  6 12:58:36 2010]
#

#
# ---------- variables --------------------------------------------------------
#

BUILD		:=		build

CONFIG		:=		config.mk

MAKE		:=		make
RM		:=		rm -Rf
DOXYGEN         :=              doxygen
BROWSER         :=              firefox

#
# ---------- includes ---------------------------------------------------------
#

-include $(CONFIG)

#
# ---------- rules ------------------------------------------------------------
#

all:			build

build::
	for i in $(BUILDDIRS); do $(MAKE) $(MAKEOPT) -C $$i; done

install:		build
	$(MAKE) -C $(BUILD) install

documentation:
	$(DOXYGEN) documentation/sources/Doxyfile

view:
	$(BROWSER) documentation/sources/html/index.html

clear:
	$(MAKE) -C $(BUILD) clean
	$(RM) $(BUILD) $(CONFIG)

clean:
	$(RM) -Rf build//*
	$(RM) -Rf dependencies/{build,work}/
