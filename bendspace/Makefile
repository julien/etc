CC=clang
INCLUDE=-I$(CPATH)
LIB=$(LIBRARY_PATH)/libraylib.a
FRAMEWORKS=-framework OpenGL -framework Cocoa -framework IOKit \
           -framework CoreAudio -framework CoreVideo -framework AudioToolbox

all:
	$(CC) bendspace.c $(INCLUDE) $(LIB) $(FRAMEWORKS) -o bendspace

