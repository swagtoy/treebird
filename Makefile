CC ?= cc
CFLAGS = -Wall
LDFLAGS = -lcurl
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
DIST = dist/
TARGET = ratfe.cgi

all: $(TARGET)

$(TARGET): filec $(OBJ)
	$(CC) -o $(DIST)$(TARGET) $(LDFLAGS) $(OBJ)

filec: src/file-to-c/main.o
	$(CC) -o file-to-c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/file-to-c/main.o

.PHONY: all filec clean
