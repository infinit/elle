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

REPOSITORY	:=		ssh://git@repositories.passeism.org

CONFIG		:=		config.mk

ECHO		:=		echo
MAKE		:=		make
TEST		:=		test
RM		:=		rm -Rf
GIT		:=		git
GREP		:=		grep
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
				hole					\
				iig					

#
# ---------- includes ---------------------------------------------------------
#

-include $(CONFIG)

#
# ---------- rules ------------------------------------------------------------
#

all:			build

build::
	$(TEST) -d $(BUILD) || $(CONFIGURE) --build-dir=$(BUILD)
	$(MAKE) $(MAKEOPT) -C $(BUILD)

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

	@for component in $(COMPONENTS); do (				\
	  echo "---[ $${component}" &&					\
	  if [ -d $${component} ] ; then				\
	    cd $${component} && $(GIT) pull ;				\
	  else								\
	    $(GIT) clone $(REPOSITORY)/$${component} ;			\
	  fi )								\
	done

status:
	@echo "---[ infinit"
	@$(GIT) status

	@for component in $(COMPONENTS); do (				\
	  echo "---[ $${component}"					; \
	  cd $${component}						; \
	  $(GIT) status	)						\
	done

commit:
	@echo "---[ infinit"
	@$(GIT) status | grep 'nothing' || $(GIT) commit -a

	@for component in $(COMPONENTS); do				\
	  echo "---[ $${component}"					; \
	  cd $${component}						; \
	  $(GIT) status | grep 'nothing' || $(GIT)  commit -a ; \
	  cd ..								; \
	done

push:
	@echo "---[ infinit"
	@$(GIT) push

	@for component in $(COMPONENTS); do				\
	  echo "---[ $${component}"					; \
	  cd $${component}						; \
	  $(GIT) push							; \
	  cd ..								; \
	done
