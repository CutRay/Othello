EXE = myothello

SRCS = \
	   myothello.c \
	   view_othello.c

OBJS = $(SRCS:.c=.o)
	CC = gcc
	#CC = clang

#上がmac,下がLinux
CFLAGS=-I/opt/X11/include -Wall
#CFLAGS=-I/usr/X11R6/include -Wall

#上がmac,下がLinux
LDFLAGS=-L/opt/X11/lib
#LDFLAGS=-L/usr/X11R6/lib

LDLIBS=-lX11

$(EXE):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS)  -o $@ $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f *~
