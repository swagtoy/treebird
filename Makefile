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
PERL_DIR = perl
PAGES = $(wildcard $(PAGES_DIR)/*.tmpl)
PERLS = $(wildcard $(PERL_DIR)/*.pl)
PAGES_CMP = $(patsubst %.tmpl,%.ctmpl,$(PAGES))
PAGES_C = $(patsubst %.tmpl, %.c,$(PAGES))
PAGES_C_OBJ = $(patsubst %.c,%.o,$(PAGES_C))
PERLS_C = $(patsubst %.pl,%.cpl,$(PERLS))
DIST = dist/
PREFIX ?= /usr/local
TARGET = treebird

$(info $(PERLS) and $(PERLS_C))

MASTODONT_URL = https://fossil.nekobit.net/mastodont-c

all: $(MASTODONT_DIR) dep_build $(TARGET)
apache: all apache_start

$(TARGET): filec template $(PERLS_C) $(PAGES_CMP) $(PAGES_C) $(PAGES_C_OBJ) $(OBJ) $(HEADERS) 
	$(CC) -o $(TARGET) $(OBJ) $(PAGES_C_OBJ) $(LDFLAGS)

template: src/template/main.o
	$(CC) $(LDFLAGS) -o template $<

filec: src/file-to-c/main.o
	$(CC) -o filec $<

emojitoc: scripts/emoji-to.o
	$(CC) -o emojitoc $< $(LDFLAGS)
	./emojitoc meta/emoji.json > src/emoji_codes.h

# Redirect stdout and stderr into separate contents as a hack
# Let bash do the work :)
$(PAGES_DIR)/%.ctmpl: $(PAGES_DIR)/%.tmpl
	./template $< $(notdir $*) 2> $(PAGES_DIR)/$(notdir $*).c 1> $@

$(PERL_DIR)/%.cpl: $(PERL_DIR)/%.pl
	./filec $< data_$(notdir $*)_pl > $@

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
