CC ?= cc
GIT ?= git
MASTODONT_DIR = mastodont-c/
MASTODONT = $(MASTODONT_DIR)libmastodont.a
CFLAGS += -Wall -I $(MASTODONT_DIR)include/ -Wno-unused-variable -Wno-discarded-qualifiers -I/usr/include/ $(shell pkg-config --cflags libcurl libcjson libpcre)
LDFLAGS = -L$(MASTODONT_DIR) -lmastodont $(shell pkg-config --libs libcjson libcurl libpcre) -lfcgi
SRC = $(wildcard src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
PAGES_DIR = static
PAGES = $(wildcard $(PAGES_DIR)/*.html)
PAGES_CMP = $(patsubst %.html,%.chtml,$(PAGES))
DIST = dist/
TARGET = treebird.cgi

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
$(PAGES_DIR)/post.chtml: $(PAGES_DIR)/post.html
	./filec $< data_post_html > $@
$(PAGES_DIR)/list.chtml: $(PAGES_DIR)/list.html
	./filec $< data_list_html > $@
$(PAGES_DIR)/lists.chtml: $(PAGES_DIR)/lists.html
	./filec $< data_lists_html > $@
$(PAGES_DIR)/error.chtml: $(PAGES_DIR)/error.html
	./filec $< data_error_html > $@
$(PAGES_DIR)/attachments.chtml: $(PAGES_DIR)/attachments.html
	./filec $< data_attachments_html > $@
$(PAGES_DIR)/attachment_image.chtml: $(PAGES_DIR)/attachment_image.html
	./filec $< data_attachment_image_html > $@
$(PAGES_DIR)/emoji_reactions.chtml: $(PAGES_DIR)/emoji_reactions.html
	./filec $< data_emoji_reactions_html > $@
$(PAGES_DIR)/emoji_reaction.chtml: $(PAGES_DIR)/emoji_reaction.html
	./filec $< data_emoji_reaction_html > $@
$(PAGES_DIR)/test.chtml: $(PAGES_DIR)/test.html
	./filec $< data_test_html > $@
$(PAGES_DIR)/notifications_page.chtml: $(PAGES_DIR)/notifications_page.html
	./filec $< data_notifications_page_html > $@
$(PAGES_DIR)/notifications.chtml: $(PAGES_DIR)/notifications.html
	./filec $< data_notifications_html > $@
$(PAGES_DIR)/notification.chtml: $(PAGES_DIR)/notification.html
	./filec $< data_notification_html > $@
$(PAGES_DIR)/notification_compact.chtml: $(PAGES_DIR)/notification_compact.html
	./filec $< data_notification_compact_html > $@
$(PAGES_DIR)/error_404.chtml: $(PAGES_DIR)/error_404.html
	./filec $< data_error_404_html > $@

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
