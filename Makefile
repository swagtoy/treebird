CC ?= cc
GIT ?= git
MASTODONT_DIR = mastodont/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS = -Wall -I $(MASTODONT)include/
LDFLAGS = -L$(MASTODONT_DIR) -lcurl -lmastodont
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
PAGES_DIR = static
PAGES = $(wildcard $(PAGES_DIR)/*.html)
PAGES_CMP = $(patsubst %.html,%.chtml,$(PAGES))
DIST = dist/
TARGET = ratfe.cgi

all: $(MASTODONT) $(TARGET)

$(TARGET): filec $(PAGES_CMP) $(OBJ)
	$(CC) -o $(DIST)$(TARGET) $(LDFLAGS) $(OBJ)

filec: src/file-to-c/main.o
	$(CC) -o filec $<

%.chtml: %.html
	./filec $< $< > $@

$(PAGES_DIR)/index.chtml: $(PAGES_DIR)/index.html
	./filec $< data_index_html > $@

$(MASTODONT):
	$(GIT) submodule update --init --recursive
	make -C $(MASTODONT_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/file-to-c/main.o
	rm -f $(PAGES_CMP)
	rm -f filec

.PHONY: all filec clean
