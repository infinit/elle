#
# ---------- header -----------------------------------------------------------
#
# project       infinite
#
# license       GPL
#
# file          /home/mycure/infinite/documentation/standards/Makefile
#
# created       julien quintard   [wed dec 19 22:28:43 2007]
# updated       julien quintard   [wed dec 19 22:28:55 2007]
#

#
# ---------- variables --------------------------------------------------------
#

PAPER			:=		standards

GARBAGE			:=		$(PAPER).aux			\
					$(PAPER).log			\
					$(PAPER).nav			\
					$(PAPER).out			\
					$(PAPER).snm			\
					$(PAPER).toc			\
					$(PAPER).dvi			\
					$(PAPER).ps			\
					$(PAPER).pdf

#
# ---------- programs ---------------------------------------------------------
#

PDFLATEX		:=		pdflatex
RM			:=		rm -f

#
# ---------- rules ------------------------------------------------------------
#

all:		build

build:
	$(PDFLATEX) $(PAPER)
	$(PDFLATEX) $(PAPER)

clean:
	@$(RM) $(GARBAGE)
	@$(RM) *~
