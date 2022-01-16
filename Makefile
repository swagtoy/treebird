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

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

filec: src/file-to-c/main.o
	$(CC) -o file-to-c $<

src/file-to-c/main.o: src/file-to-c/main.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all
