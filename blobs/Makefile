CC=clang
INCLUDE=-I$(HOME)/.local/glfw/include
LIB=$(HOME)/.local/glfw/lib/libglfw3.a
FRAMEWORKS=-framework OpenGL -framework Cocoa -framework IOKit \
           -framework CoreAudio -framework CoreVideo -framework AudioToolbox

all:
	$(CC) blobs.c $(INCLUDE) $(LIB) $(FRAMEWORKS) -o blobs

