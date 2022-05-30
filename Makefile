CC ?= cc
GIT ?= git
MASTODONT_DIR = mastodont-c/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS += -Wall -I $(MASTODONT_DIR)include/ -Wno-unused-variable -Wno-ignored-qualifiers -I/usr/include/ $(shell pkg-config --cflags libcurl libcjson libpcre)
LDFLAGS = -L$(MASTODONT_DIR) -lmastodont $(shell pkg-config --libs libcjson libcurl libpcre) -lfcgi
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
HEADERS = $(wildcard src/*.h)
PAGES_DIR = static
PAGES = $(wildcard $(PAGES_DIR)/*.tmpl)
PAGES_CMP = $(patsubst %.tmpl,%.ctmpl,$(PAGES))
DIST = dist/
PREFIX ?= /usr/local
TARGET = treebird

MASTODONT_URL = https://fossil.nekobit.net/mastodont-c

all: $(MASTODONT_DIR) dep_build $(TARGET)
apache: all apache_start

$(TARGET): filec template $(PAGES_CMP) $(OBJ) $(HEADERS)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

template: src/template/main.o
	$(CC) $(LDFLAGS) -o template $<

filec: src/file-to-c/main.o
	$(CC) -o filec $<

emojitoc: scripts/emoji-to.o
	$(CC) -o emojitoc $< $(LDFLAGS)
	./emojitoc meta/emoji.json > src/emoji_codes.h

$(PAGES_DIR)/%.ctmpl: $(PAGES_DIR)/%.tmpl
	./template $< $(notdir $*) > $@

$(MASTODONT_DIR): 
	cd ..; fossil clone $(MASTODONT_URL) || true
	cd treebird; ln -s ../mastodont-c .

install: $(TARGET)
	install -m 755 treebird $(PREFIX)/bin/
	install -d $(PREFIX)/share/treebird/
	cp -r dist/ $(PREFIX)/share/treebird/

test:
	make -C test

apache_start:
	./scripts/fcgistarter.sh

dep_build:
	make -C $(MASTODONT_DIR)

%.o: %.c %.h $(PAGES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/file-to-c/main.o
	rm -f $(PAGES_CMP)
	rm -f filec
	make -C $(MASTODONT_DIR) clean

clean_deps:
	rm -r $(MASTODONT_DIR)

clean_all: clean clean_deps

.PHONY: all filec clean update clean clean_deps clean_all test
