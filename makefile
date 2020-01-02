EXE = myothello

SRCS = \
	   myothello.c \
	   view_othello.c

OBJS = $(SRCS:.c=.o)
	CC = gcc
	#CC = clang

CFLAGS=-I/opt/X11/include -Wall
LDFLAGS=-L/opt/X11/lib
LDLIBS=-lX11

$(EXE):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS)  -o $@ $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f *~
