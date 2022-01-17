CC ?= cc
CFLAGS = -Wall
LDFLAGS = -lcurl
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
PAGES_DIR = static
PAGES = $(wildcard $(PAGES_DIR)/*.html)
PAGES_CMP = $(patsubst %.html,%.chtml,$(PAGES))
DIST = dist/
TARGET = ratfe.cgi

all: $(TARGET)

$(TARGET): filec $(PAGES_CMP) $(OBJ)
	$(CC) -o $(DIST)$(TARGET) $(LDFLAGS) $(OBJ)

filec: src/file-to-c/main.o
	$(CC) -o file-to-c $<

%.chtml: %.html
	./file-to-c $< $< > $@

$(PAGES_DIR)/index.chtml: $(PAGES_DIR)/index.html
	./file-to-c $< data_index_html > $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/file-to-c/main.o
	rm -f $(PAGES_CMP)

.PHONY: all filec clean
