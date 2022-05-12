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
PAGES = $(wildcard $(PAGES_DIR)/*.html)
PAGES_CMP = $(patsubst %.html,%.chtml,$(PAGES))
DIST = dist/
PREFIX ?= /usr/local
TARGET = treebird

MASTODONT_URL = https://fossil.nekobit.net/mastodont-c

all: $(MASTODONT_DIR) dep_build $(TARGET)
apache: all apache_start

$(TARGET): filec $(PAGES_CMP) $(OBJ) $(HEADERS)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

filec: src/file-to-c/main.o
	$(CC) -o filec $<

# PAGES
$(PAGES_DIR)/index.chtml: $(PAGES_DIR)/index.html
	./filec $< data_index_html > $@
$(PAGES_DIR)/status.chtml: $(PAGES_DIR)/status.html
	./filec $< data_status_html > $@
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
$(PAGES_DIR)/navigation.chtml: $(PAGES_DIR)/navigation.html
	./filec $< data_navigation_html > $@
$(PAGES_DIR)/config_sidebar.chtml: $(PAGES_DIR)/config_sidebar.html
	./filec $< data_config_sidebar_html > $@
$(PAGES_DIR)/like_svg.chtml: $(PAGES_DIR)/like_svg.html
	./filec $< data_like_svg_html > $@
$(PAGES_DIR)/repeat_svg.chtml: $(PAGES_DIR)/repeat_svg.html
	./filec $< data_repeat_svg_html > $@
$(PAGES_DIR)/follow_svg.chtml: $(PAGES_DIR)/follow_svg.html
	./filec $< data_follow_svg_html > $@
$(PAGES_DIR)/notification_action.chtml: $(PAGES_DIR)/notification_action.html
	./filec $< data_notification_action_html > $@
$(PAGES_DIR)/config_general.chtml: $(PAGES_DIR)/config_general.html
	./filec $< data_config_general_html > $@
$(PAGES_DIR)/config_appearance.chtml: $(PAGES_DIR)/config_appearance.html
	./filec $< data_config_appearance_html > $@
$(PAGES_DIR)/in_reply_to.chtml: $(PAGES_DIR)/in_reply_to.html
	./filec $< data_in_reply_to_html > $@
$(PAGES_DIR)/account_info.chtml: $(PAGES_DIR)/account_info.html
	./filec $< data_account_info_html > $@
$(PAGES_DIR)/search.chtml: $(PAGES_DIR)/search.html
	./filec $< data_search_html > $@
$(PAGES_DIR)/scrobble.chtml: $(PAGES_DIR)/scrobble.html
	./filec $< data_scrobble_html > $@
$(PAGES_DIR)/directs_page.chtml: $(PAGES_DIR)/directs_page.html
	./filec $< data_directs_page_html > $@
$(PAGES_DIR)/status_interactions.chtml: $(PAGES_DIR)/status_interactions.html
	./filec $< data_status_interactions_html > $@
$(PAGES_DIR)/status_interactions_label.chtml: $(PAGES_DIR)/status_interactions_label.html
	./filec $< data_status_interactions_label_html > $@
$(PAGES_DIR)/status_interaction_profile.chtml: $(PAGES_DIR)/status_interaction_profile.html
	./filec $< data_status_interaction_profile_html > $@
$(PAGES_DIR)/account_follow_btn.chtml: $(PAGES_DIR)/account_follow_btn.html
	./filec $< data_account_follow_btn_html > $@
$(PAGES_DIR)/bookmarks_page.chtml: $(PAGES_DIR)/bookmarks_page.html
	./filec $< data_bookmarks_page_html > $@
$(PAGES_DIR)/favourites_page.chtml: $(PAGES_DIR)/favourites_page.html
	./filec $< data_favourites_page_html > $@
$(PAGES_DIR)/account_stub.chtml: $(PAGES_DIR)/account_stub.html
	./filec $< data_account_stub_html > $@
$(PAGES_DIR)/hashtag.chtml: $(PAGES_DIR)/hashtag.html
	./filec $< data_hashtag_html > $@
$(PAGES_DIR)/hashtag_page.chtml: $(PAGES_DIR)/hashtag_page.html
	./filec $< data_hashtag_page_html > $@
$(PAGES_DIR)/bar_graph.chtml: $(PAGES_DIR)/bar_graph.html
	./filec $< data_bar_graph_html > $@
$(PAGES_DIR)/bar.chtml: $(PAGES_DIR)/bar.html
	./filec $< data_bar_html > $@
$(PAGES_DIR)/emoji_picker.chtml: $(PAGES_DIR)/emoji_picker.html
	./filec $< data_emoji_picker_html > $@
$(PAGES_DIR)/emoji.chtml: $(PAGES_DIR)/emoji.html
	./filec $< data_emoji_html > $@

$(MASTODONT_DIR): 
	fossil clone $(MASTODONT_URL) || true

install: $(TARGET)
	install -m 655 treebird $(PREFIX)/bin/
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
