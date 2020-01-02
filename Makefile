# Variable definitions.

PLATFORM	= $(shell uname)
CC		= gcc
OBJDIR		= ./objs
TESTS		= ./tests
TESTS_MO    = ./tests_modifies
TESTS_MA    = ./tests_maison
INCLUDES	= ./include
INCFLAGS	:= -I${INCLUDES} -I${TESTS}
#OPTFLAGS	:= -Os -DNDEBUG
OPTFLAGS	:= -g
CCFLAGS		:= -c ${OPTFLAGS} -Wall -std=c99

# The list of objects to include in the library

# AJOUTER LES FICHIERS OBJETS ASSOCIES AUX FICHIERS SRC ICI
LIBEIOBJS	:= objs/draw_lines.o objs/copy_text.o objs/polygone.o objs/ei_widgetclass.o  objs/ei_application.o objs/arc.o objs/ei_widget.o objs/ei_placer.o objs/ei_event.o



# Platform specific definitions (OS X, Linux)

ifeq (${PLATFORM},Darwin)

	# Building for Mac OS X

	PLATDIR		= _osx
	INCFLAGS	:= ${INCFLAGS} -I/opt/local/include/SDL
	LINK		= ${CC}
	LIBEI		= ${OBJDIR}/libei.a
	LIBEIBASE	= ${PLATDIR}/libeibase.a
	LIBS		= ${LIBEIBASE} -L/opt/local/lib -lSDL -lSDL_ttf -lSDL_image -framework AppKit
	CCFLAGS		:= ${CCFLAGS} -D__OSX__

else

	# Building for Linux

	PLATDIR		= _x11
	INCFLAGS	:= ${INCFLAGS} -I/usr/include/SDL
	LINK		= ${CC}
#	ARCH	        = 32
	ARCH	        = 64
	LIBEI		= ${OBJDIR}/libei.a
	LIBEIBASE	= ${PLATDIR}/libeibase${ARCH}.a
	LIBS		= ${LIBEIBASE} -lSDL -lSDL_ttf -lSDL_image -lm
	CCFLAGS		:= ${CCFLAGS} -D__LINUX__ -m${ARCH}
	LDFLAGS		= -m${ARCH} -g

endif



# Main target of the makefile. To build specific targets, call "make <target_name>"
#
# AJOUTER LES NOMS DES FICHIERS DE TEST
TARGETS		=	${LIBEI} \
			minimal lines frame button hello_world puzzle two048 test_polygone text test_frame test_arc frame_mod updated_frame
all : ${TARGETS}



########## Test-programs

#objs/%.o:
#	gcc -c -g -o  objs/$@.o  -I./include -I./tests -I/usr/include/SDL src/ei_draw.c

$(OBJDIR)/%.o: src/%.c
	$(LINK) -c -g -o  $@ $(CFLAGS) $(INCFLAGS) -I/usr/include/SDL $<

# minimal

minimal : ${OBJDIR}/minimal.o ${LIBEIBASE}
	${LINK} -o minimal ${LDFLAGS} ${OBJDIR}/minimal.o ${LIBS}

${OBJDIR}/minimal.o : ${TESTS}/minimal.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/minimal.c -o ${OBJDIR}/minimal.o

# test_frame

test_frame : ${OBJDIR}/test_frame.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o test_frame ${OBJDIR}/test_frame.o ${LIBEI} ${LIBS}

${OBJDIR}/test_frame.o : ${TESTS_MA}/test_frame.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS_MA}/test_frame.c -o ${OBJDIR}/test_frame.o


#updated_frame

updated_frame : ${OBJDIR}/updated_frame.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o updated_frame ${OBJDIR}/updated_frame.o ${LIBEI} ${LIBS}

${OBJDIR}/updated_frame.o : ${TESTS_MO}/updated_frame.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS_MO}/updated_frame.c -o ${OBJDIR}/updated_frame.o

#frame_mod

frame_mod : ${OBJDIR}/frame_mod.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o frame_mod ${OBJDIR}/frame_mod.o ${LIBEI} ${LIBS}

${OBJDIR}/frame_mod.o : ${TESTS_MO}/frame_mod.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS_MO}/frame_mod.c -o ${OBJDIR}/frame_mod.o


# lines

lines : ${OBJDIR}/lines.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o lines ${OBJDIR}/lines.o ${LIBEI} ${LIBS}

${OBJDIR}/lines.o : ${TESTS}/lines.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/lines.c -o ${OBJDIR}/lines.o

# text

text : ${OBJDIR}/text.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o text ${OBJDIR}/text.o ${LIBEI} ${LIBS}

${OBJDIR}/text.o : ${TESTS_MA}/text.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS_MA}/text.c -o ${OBJDIR}/text.o

# test_arc

test_arc : ${OBJDIR}/test_arc.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o test_arc ${OBJDIR}/test_arc.o ${LIBEI} ${LIBS}

${OBJDIR}/test_arc.o : ${TESTS_MA}/test_arc.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS_MA}/test_arc.c -o ${OBJDIR}/test_arc.o

# frame

frame : ${OBJDIR}/frame.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o frame ${LDFLAGS} ${OBJDIR}/frame.o ${LIBEI} ${LIBS}

${OBJDIR}/frame.o : ${TESTS}/frame.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/frame.c -o ${OBJDIR}/frame.o

# button

button : ${OBJDIR}/button.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o button ${LDFLAGS} ${OBJDIR}/button.o ${LIBEI} ${LIBS}

${OBJDIR}/button.o : ${TESTS}/button.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/button.c -o ${OBJDIR}/button.o

# hello_world

hello_world : ${OBJDIR}/hello_world.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o hello_world ${LDFLAGS} ${OBJDIR}/hello_world.o ${LIBEI} ${LIBS}

${OBJDIR}/hello_world.o : ${TESTS}/hello_world.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/hello_world.c -o ${OBJDIR}/hello_world.o

# puzzle

puzzle : ${OBJDIR}/puzzle.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o puzzle ${LDFLAGS} ${OBJDIR}/puzzle.o ${LIBEI} ${LIBS}

${OBJDIR}/puzzle.o : ${TESTS}/puzzle.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/puzzle.c -o ${OBJDIR}/puzzle.o

# test_polygone

test_polygone: ${OBJDIR}/test_polygone.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o test_polygone ${LDFLAGS} ${OBJDIR}/test_polygone.o ${LIBEI} ${LIBS}

${OBJDIR}/test_polygone.o : ${TESTS_MA}/test_polygone.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS_MA}/test_polygone.c -o ${OBJDIR}/test_polygone.o

# two048

two048 : ${OBJDIR}/two048.o ${LIBEIBASE} ${LIBEI}
	${LINK} -o two048 ${LDFLAGS} ${OBJDIR}/two048.o ${LIBEI} ${LIBS}

${OBJDIR}/two048.o : ${TESTS}/two048.c
	${CC} ${CCFLAGS} ${INCFLAGS} ${TESTS}/two048.c -o ${OBJDIR}/two048.o



# Building of the library libei

${LIBEI} : ${LIBEIOBJS}
	ar rcs ${LIBEI} ${LIBEIOBJS}



# Building of the doxygen documentation.

doc :
	doxygen docs/doxygen.cfg



# Removing all built files.

clean:
	rm -f ${TARGETS}
	rm -f *.exe
	rm -f ${OBJDIR}/*
