CC ?= cc
CFLAGS = -Wall
LDFLAGS = -lcurl
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
TARGET = ratfe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(LDFLAGS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all
