#
# ---------- header -----------------------------------------------------------
#
# project       infinite
#
# license       GPL
#
# file          /home/mycure/pers...tudies/ucam/phd/development/pig/Makefile
#
# created       julien quintard   [mon oct 22 12:13:12 2007]
# updated       julien quintard   [tue oct 23 18:09:42 2007]
#

#
# ---------- programs ---------------------------------------------------------
#

CC		:=		gcc
RM		:=		rm -f

#
# ---------- variables --------------------------------------------------------
#

TARGET		:=		pig

SOURCES		:=		pig.c					
OBJECTS		:=		$(SOURCES:.c=.o)

LDFLAGS		:=		-pthread				\
				-lrt					\
				-lfuse					

CFLAGS		:=		-I/usr/include/fuse			\
				-D_FILE_OFFSET_BITS=64			\
				-Wall					

#
# ---------- rules ------------------------------------------------------------
#

main:		$(TARGET)

$(TARGET):	$(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $(TARGET)

clear:
	$(RM) $(OBJECTS) *~

clean:		clear
	$(RM) $(TARGET)

%.o:		%.c
	$(CC) $(CFLAGS) $< -c -o $@
