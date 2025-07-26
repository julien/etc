CC=clang
INCLUDE=-I$(HOME)/.local/raylib/include
LIB=$(HOME)/.local/raylib/lib/libraylib.a
FRAMEWORKS=-framework OpenGL -framework Cocoa -framework IOKit \
           -framework CoreAudio -framework CoreVideo -framework AudioToolbox

all:
	$(CC) main.c $(INCLUDE) $(LIB) $(FRAMEWORKS) -o main

