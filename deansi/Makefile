CC = cc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -fsanitize=address
TARGET = deansi

all: $(TARGET)

$(TARGET): deansi.c
	$(CC) $(CFLAGS) -o $(TARGET) deansi.c

clean:
	rm -f $(TARGET)

.PHONY: all clean
