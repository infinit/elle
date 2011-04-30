#
# ---------- header -----------------------------------------------------------
#
# project       infinit
#
# license       infinit
#
# file          /home/mycure/repositories/XXX/infinit/Makefile
#
# created       julien quintard   [wed oct  6 12:58:36 2010]
# updated       julien quintard   [fri apr 29 23:28:33 2011]
#

#
# ---------- variables --------------------------------------------------------
#

BUILD		:=		build

REPOSITORY	:=		ssh://git@repositories.passeism.org

CONFIG		:=		config.mk

MAKE		:=		make
TEST		:=		test
RM		:=		rm -Rf
GIT		:=		git
GREP		:=		grep -v
WC		:=		wc -l

CONFIGURE	:=		./configure

COMPONENTS	:=		elle					\
				comet					\
				nucleus					\
				etoile					\
				agent					\
				pig					\
				applications				\
				lune					\
				hole

#
# ---------- includes ---------------------------------------------------------
#

-include $(CONFIG)

#
# ---------- rules ------------------------------------------------------------
#

all:			build

build:
	$(TEST) -d $(BUILD) || $(CONFIGURE) --build-dir=$(BUILD)
	$(MAKE) -C $(BUILD)

install:		build
	$(MAKE) -C $(BUILD) install

documentation:
#	$(DOXYGEN) doc/Doxyfile

clean:
	$(MAKE) -C $(BUILD) clean
	$(RM) $(BUILD) $(CONFIG)

pull:
	@echo "---[ infinit"
	@$(GIT) pull

	@for component in $(COMPONENTS); do				\
	  echo "---[ $${component}"					&& \
	  if [ -d $${component} ] ; then				\
	    cd $${component}						&& \
	    $(GIT) pull							&& \
	    cd ..							; \
	  else								\
	    $(GIT) clone $(REPOSITORY)/$${component}			; \
	  fi								\
	done

status:
	@echo "---[ infinit"
	@$(GIT) status

	@for component in $(COMPONENTS); do				\
	  echo "---[ $${component}"					&& \
	  cd $${component}						&& \
	  $(GIT) status							&& \
	  cd ..								; \
	done

push:
	@echo "---[ infinit"
	@$(GIT) commit -a && $(GIT) push

	@for component in $(COMPONENTS); do				\
	  echo "---[ $${component}"					&& \
	  cd $${component}						&& \
	  $(GIT) commit -a && $(GIT) push				&& \
	  cd ..								; \
	done
