CC ?= cc
GIT ?= git
MASTODONT_DIR = mastodont-c/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS += -Wall -I $(MASTODONT_DIR)include/
LDFLAGS = -L$(MASTODONT_DIR) -lcurl -lmastodont -lcjson
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
PAGES_DIR = static
PAGES = $(wildcard $(PAGES_DIR)/*.html)
PAGES_CMP = $(patsubst %.html,%.chtml,$(PAGES))
DIST = dist/
TARGET = ratfe.cgi

MASTODONT_URL = https://git.nekobit.net/repos/mastodont-c.git

all: $(MASTODONT_DIR) dep_build $(TARGET)

$(TARGET): filec $(PAGES_CMP) $(OBJ)
	$(CC) -o $(DIST)$(TARGET) $(OBJ) $(LDFLAGS)

filec: src/file-to-c/main.o
	$(CC) -o filec $<

%.chtml: %.html
	./filec $< $< > $@

# PAGES
$(PAGES_DIR)/index.chtml: $(PAGES_DIR)/index.html
	./filec $< data_index_html > $@
$(PAGES_DIR)/status.chtml: $(PAGES_DIR)/status.html
	./filec $< data_status_html > $@
$(PAGES_DIR)/config.chtml: $(PAGES_DIR)/config.html
	./filec $< data_config_html > $@
$(PAGES_DIR)/account.chtml: $(PAGES_DIR)/account.html
	./filec $< data_account_html > $@
$(PAGES_DIR)/login.chtml: $(PAGES_DIR)/login.html
	./filec $< data_login_html > $@

$(MASTODONT_DIR): 
	git clone $(MASTODONT_URL) || true
	@echo -e "\033[38;5;13mRun 'make update' to update mastodont-c\033[0m"

dep_build:
	make -C $(MASTODONT_DIR)

update:
	git -C $(MASTODONT_DIR) pull

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) src/file-to-c/main.o
	rm -f $(PAGES_CMP)
	rm -f filec
	make -C $(MASTODONT_DIR) clean

clean_deps:
	rm -r $(MASTODONT_DIR)

clean_all: clean clean_deps

.PHONY: all filec clean update clean clean_deps clean_all
