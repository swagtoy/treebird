/*
 * Treebird - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "global_perl.h"

/* TODO let's generate this file dynamically with a Perl script? */
#include "../templates/main.ctt"
#include "../templates/notification.ctt"
#include "../templates/status.ctt"
#include "../templates/content_status.ctt"
#include "../templates/emoji_picker.ctt"
#include "../templates/attachment.ctt"
#include "../templates/emoji.ctt"
#include "../templates/postbox.ctt"
#include "../templates/timeline.ctt"
#include "../templates/account.ctt"
#include "../templates/account_statuses.ctt"
#include "../templates/account_scrobbles.ctt"
#include "../templates/content_notifs.ctt"
#include "../templates/content_lists.ctt"
#include "../templates/navigation.ctt"
#include "../templates/accounts.ctt"
#include "../templates/account_item.ctt"
#include "../templates/content_search.ctt"
#include "../templates/search_accounts.ctt"
#include "../templates/search_statuses.ctt"
#include "../templates/search_tags.ctt"
#include "../templates/search.ctt"
#include "../templates/content_chats.ctt"
#include "../templates/chat.ctt"
#include "../templates/config_general.ctt"
#include "../templates/config_appearance.ctt"
#include "../templates/embed.ctt"
#include "../templates/notifs_embed.ctt"
#include "../templates/about.ctt"
#include "../templates/license.ctt"

PerlInterpreter* my_perl;
HV* template_files;
pthread_mutex_t perllock_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_template_files(pTHX)
{
    template_files = newHV();

    hv_stores(template_files, "main.tt", newSVpv(data_main_tt, data_main_tt_size));
    hv_stores(template_files, "notification.tt", newSVpv(data_notification_tt, data_notification_tt_size));
    hv_stores(template_files, "status.tt", newSVpv(data_status_tt, data_status_tt_size));
    hv_stores(template_files, "content_status.tt", newSVpv(data_content_status_tt, data_content_status_tt_size));
    hv_stores(template_files, "emoji_picker.tt", newSVpv(data_emoji_picker_tt, data_emoji_picker_tt_size));
    hv_stores(template_files, "attachment.tt", newSVpv(data_attachment_tt, data_attachment_tt_size));
    hv_stores(template_files, "emoji.tt", newSVpv(data_emoji_tt, data_emoji_tt_size));
    hv_stores(template_files, "postbox.tt", newSVpv(data_postbox_tt, data_postbox_tt_size));
    hv_stores(template_files, "timeline.tt", newSVpv(data_timeline_tt, data_timeline_tt_size));
    hv_stores(template_files, "account.tt", newSVpv(data_account_tt, data_account_tt_size));
    hv_stores(template_files, "account_statuses.tt", newSVpv(data_account_statuses_tt, data_account_statuses_tt_size));
    hv_stores(template_files, "account_scrobbles.tt", newSVpv(data_account_scrobbles_tt, data_account_scrobbles_tt_size));
    hv_stores(template_files, "content_notifs.tt", newSVpv(data_content_notifs_tt, data_content_notifs_tt_size));
    hv_stores(template_files, "content_lists.tt", newSVpv(data_content_lists_tt, data_content_lists_tt_size));
    hv_stores(template_files, "navigation.tt", newSVpv(data_navigation_tt, data_navigation_tt_size));
    hv_stores(template_files, "accounts.tt", newSVpv(data_accounts_tt, data_accounts_tt_size));
    hv_stores(template_files, "account_item.tt", newSVpv(data_account_item_tt, data_account_item_tt_size));
    hv_stores(template_files, "content_search.tt", newSVpv(data_content_search_tt, data_content_search_tt_size));
    hv_stores(template_files, "search.tt", newSVpv(data_search_tt, data_search_tt_size));
    hv_stores(template_files, "search_accounts.tt", newSVpv(data_search_accounts_tt, data_search_accounts_tt_size));
    hv_stores(template_files, "search_statuses.tt", newSVpv(data_search_statuses_tt, data_search_statuses_tt_size));
    hv_stores(template_files, "search_tags.tt", newSVpv(data_search_tags_tt, data_search_tags_tt_size));
    hv_stores(template_files, "content_chats.tt", newSVpv(data_content_chats_tt, data_content_chats_tt_size));
    hv_stores(template_files, "chat.tt", newSVpv(data_chat_tt, data_chat_tt_size));
    hv_stores(template_files, "config_general.tt", newSVpv(data_config_general_tt, data_config_general_tt_size));
    hv_stores(template_files, "config_appearance.tt", newSVpv(data_config_appearance_tt, data_config_appearance_tt_size));
    hv_stores(template_files, "embed.tt", newSVpv(data_embed_tt, data_embed_tt_size));
    hv_stores(template_files, "notifs_embed.tt", newSVpv(data_notifs_embed_tt, data_notifs_embed_tt_size));
    hv_stores(template_files, "about.tt", newSVpv(data_about_tt, data_about_tt_size));
    hv_stores(template_files, "license.tt", newSVpv(data_license_tt, data_license_tt_size));
}

void cleanup_template_files()
{
    hv_undef(template_files);
}
