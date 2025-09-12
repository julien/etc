CC=clang
INCLUDE=-I$(CPATH)
LIB=$(LIBRARY_PATH)/libglfw3.a
FRAMEWORKS=-framework OpenGL -framework Cocoa -framework IOKit \
           -framework CoreAudio -framework CoreVideo -framework AudioToolbox

all:
	$(CC) blobs.c $(INCLUDE) $(LIB) $(FRAMEWORKS) -o blobs

