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
#include "../templates/notif_compact.ctt"
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

PerlInterpreter* my_perl;
HV* template_files;
pthread_mutex_t perllock_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_template_files(pTHX)
{
    template_files = newHV();

    hv_stores(template_files, "main.tt", newSVpv(data_main_tt, data_main_tt_size));
    hv_stores(template_files, "notif_compact.tt", newSVpv(data_notif_compact_tt, data_notif_compact_tt_size));
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
}

void cleanup_template_files()
{
    hv_undef(template_files);
}
