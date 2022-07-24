CC ?= cc
GIT ?= git
MASTODONT_DIR = mastodont-c/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS += -Wall -I $(MASTODONT_DIR)include/ -Wno-unused-variable -Wno-ignored-qualifiers -I/usr/include/ -I $(MASTODONT_DIR)/libs $(shell pkg-config --cflags libcurl libpcre2-8) `perl -MExtUtils::Embed -e ccopts`
LDFLAGS = -L$(MASTODONT_DIR) -lmastodont $(shell pkg-config --libs libcurl libpcre2-8) -lfcgi -lpthread `perl -MExtUtils::Embed -e ldopts`
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
HEADERS = $(wildcard src/*.h) config.h
PAGES_DIR = static
TMPL_DIR = templates
PAGES = $(wildcard $(PAGES_DIR)/*.tmpl)
TMPLS = $(wildcard $(TMPL_DIR)/*.tt)
PAGES_CMP = $(patsubst %.tmpl,%.ctmpl,$(PAGES))
PAGES_C = $(patsubst %.tmpl, %.c,$(PAGES))
PAGES_C_OBJ = $(patsubst %.c,%.o,$(PAGES_C))
TMPLS_C = $(patsubst %.tt,%.ctt,$(TMPLS))
DIST = dist/
PREFIX ?= /usr/local
TARGET = treebird

MASTODONT_URL = https://fossil.nekobit.net/mastodont-c

all: $(MASTODONT_DIR) dep_build $(TARGET)
apache: all apache_start

$(TARGET): filec ctemplate $(TMPLS_C) $(TMPLS) $(PAGES_CMP) $(PAGES_C) $(PAGES_C_OBJ) $(OBJ) $(HEADERS) 
	$(CC) -o $(TARGET) $(OBJ) $(PAGES_C_OBJ) $(LDFLAGS)

filec: src/file-to-c/main.o
	$(CC) -o filec $<

emojitoc: scripts/emoji-to.o
	$(CC) -o emojitoc $< $(LDFLAGS)
	./emojitoc meta/emoji.json > src/emoji_codes.h

# Redirect stdout and stderr into separate contents as a hack
# Let bash do the work :)
$(PAGES_DIR)/%.ctmpl: $(PAGES_DIR)/%.tmpl $(TMPLS)
	./ctemplate $< $(notdir $*) 2> $(PAGES_DIR)/$(notdir $*).c 1> $@

$(TMPL_DIR)/%.ctt: $(TMPL_DIR)/%.tt
	./filec $< data_$(notdir $*)_tt > $@

ctemplate: src/template/main.o
	$(CC) $(LDFLAGS) -o ctemplate $<

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
	rm -f $(TMPLS_C)
	rm -f filec
	make -C $(MASTODONT_DIR) clean

clean_deps:
	rm -r $(MASTODONT_DIR)

clean_all: clean clean_deps

.PHONY: all filec clean update clean clean_deps clean_all test
