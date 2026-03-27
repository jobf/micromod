
CC=gcc
CFLAGS=-ansi -pedantic -Wall -Og

all: xm2wav

clean:
	rm -f xm2wav ibxm-sdl

xm2wav: xm2wav.c ibxm.c ibxm.h
	$(CC) $(CFLAGS) xm2wav.c ibxm.c -o xm2wav

ibxm-sdl: sdlplayer.c patterndisplay.c patterndisplay.h topaz8.c ibxm.c ibxm.h
	$(CC) $(CFLAGS) sdlplayer.c patterndisplay.c topaz8.c ibxm.c -o ibxm-sdl `sdl2-config --cflags --libs`

ibxm-portable: player.c patterndisplay.c patterndisplay.h topaz8.c ibxm.c ibxm.h portable.h
	$(CC) $(CFLAGS) player.c patterndisplay.c topaz8.c ibxm.c -o ibxm-portable `sdl2-config --cflags --libs`