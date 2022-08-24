CC ?= cc
GIT ?= git
MASTODONT_DIR = mastodont-c/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS += -Wall -I $(MASTODONT_DIR)include/ -Wno-unused-variable -Wno-ignored-qualifiers -I/usr/include/ -I $(MASTODONT_DIR)/libs $(shell pkg-config --cflags libcurl libpcre2-8) `perl -MExtUtils::Embed -e ccopts`
LDFLAGS = -L$(MASTODONT_DIR) -lmastodont $(shell pkg-config --libs libcurl libpcre2-8) -lfcgi -lpthread `perl -MExtUtils::Embed -e ldopts`
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
HEADERS = $(wildcard src/*.h) config.h
TMPL_DIR = templates
TMPLS = $(wildcard $(TMPL_DIR)/*.tt)
TMPLS_C = $(patsubst %.tt,%.ctt,$(TMPLS))
TEST_DIR = test/unit
TESTS = $(wildcard $(TEST_DIR)/t*.c)
UNIT_TESTS = $(patsubst %.c,%.bin,$(TESTS))
DIST = dist/
PREFIX ?= /usr/local
TARGET = treebird
# For tests
OBJ_NO_MAIN = $(filter-out src/main.o,$(OBJ))

MASTODONT_URL = https://fossil.nekobit.net/mastodont-c

# Not parallel friendly
#all: $(MASTODONT_DIR) dep_build $(TARGET)

ifneq ($(strip $(SINGLE_THREADED)),)
CFLAGS += -DSINGLE_THREADED
endif

all:
	$(MAKE) dep_build
	$(MAKE) filec
	$(MAKE) make_tmpls
	$(MAKE) $(TARGET)

install_deps:
	cpan Template::Toolkit

$(TARGET): $(HEADERS) $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(PAGES_C_OBJ) $(LDFLAGS)

filec: src/file-to-c/main.o
	$(CC) -o filec $<

emojitoc: scripts/emoji-to.o
	$(CC) -o emojitoc $< $(LDFLAGS)
	./emojitoc meta/emoji.json > src/emoji_codes.h

$(TMPL_DIR)/%.ctt: $(TMPL_DIR)/%.tt
	./filec $< data_$(notdir $*)_tt > $@

make_tmpls: $(TMPLS_C)

$(MASTODONT_DIR): 
	cd ..; fossil clone $(MASTODONT_URL) || true
	cd treebird; ln -s ../mastodont-c .

install: $(TARGET)
	install -m 755 treebird $(PREFIX)/bin/
	install -d $(PREFIX)/share/treebird/
	cp -r dist/ $(PREFIX)/share/treebird/

test: all $(UNIT_TESTS)
	@echo " ... Tests ready"
	@./test/test.pl

dep_build:
	make -C $(MASTODONT_DIR)

%.o: %.c %.h $(PAGES)
	$(CC) $(CFLAGS) -c $< -o $@

# For tests
%.bin: %.c
	@$(CC) $(CFLAGS) $< -o $@ $(OBJ_NO_MAIN) $(PAGES_C_OBJ) $(LDFLAGS)
	@echo -n " $@"

clean:
	rm -f $(OBJ) src/file-to-c/main.o
	rm -f $(TMPLS_C)
	rm -f test/unit/*.bin
	rm -f filec ctemplate
	rm $(TARGET) || true
	make -C $(MASTODONT_DIR) clean

clean_deps:
	rm -r $(MASTODONT_DIR)

clean_all: clean clean_deps

.PHONY: all filec clean update clean clean_deps clean_all test install_deps
