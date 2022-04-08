CC ?= cc
MASTODONT_DIR = mastodont-c/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS += -Wall -I ../$(MASTODONT_DIR)include/ -Wno-unused-variable -Wno-discarded-qualifiers -I/usr/include/ $(shell pkg-config --cflags libcurl libcjson libpcre)
LDFLAGS = -L./../$(MASTODONT_DIR) -lmastodont $(shell pkg-config --libs libcjson libcurl libpcre) -lfcgi
TARGET = tests
SRC = unit/main.c
OBJ = $(patsubst %.c,%.o,$(SRC)) 

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
