all:

BUILDDIR = build
MAKEOPT = --quiet
GIT_REPO = git@repositories.i.nfin.it:/data/git/mycure/repositories

-include config.mk

all:
	test -d $(BUILDDIR) || ./configure --build-dir=$(BUILDDIR)
	make $(MAKEOPT) -C $(BUILDDIR)

install: all
	make $(MAKEOPT) -C $(BUILDDIR) install

doc:
#	$(DOXYGEN) doc/Doxyfile && cp doc/tabs.css doc/html/

check: install
	@echo
	@echo "    ---------------------------------"
	@echo "    |          TEST SUITE           |"
	@echo "    ---------------------------------"
	@echo

clean:
	make $(MAKEOPT) -C $(BUILDDIR) clean
	find . \( -name '*~' \) -exec rm -f {} \;

distclean:
	rm -rf $(BUILDDIR) config.mk

.PHONY: doc help

help:
	@echo "build		build the project"
	@echo "check		check the project"
	@echo "clean		clean the project"
	@echo "distclean	distclean the project"
	@echo "doc		generate the doxygen html documentation"
	@echo "pull             will pull or clone the repository"

pull:
	for i in agent core elle hole pig; \
do \
  if [[ -d $$i ]] ; then \
    (cd $$i && git pull) \
  else \
    git clone $(GIT_REPO)/$$i.git; \
  fi; \
done
