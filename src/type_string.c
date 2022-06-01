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

#include "type_string.h"

// Icons
#include "../static/like_svg.ctmpl"
#include "../static/repeat_svg.ctmpl"
#include "../static/follow_svg.ctmpl"

const char* notification_type_str(mstdnt_notification_t type)
{
    switch (type)
    {
    case MSTDNT_NOTIFICATION_FOLLOW: return L10N[L10N_EN_US][L10N_NOTIF_FOLLOW];
    case MSTDNT_NOTIFICATION_FOLLOW_REQUEST: return L10N[L10N_EN_US][L10N_NOTIF_FOLLOW_REQUEST];
    case MSTDNT_NOTIFICATION_REBLOG: return L10N[L10N_EN_US][L10N_NOTIF_REPEATED];
    case MSTDNT_NOTIFICATION_FAVOURITE: return L10N[L10N_EN_US][L10N_NOTIF_LIKED];
    case MSTDNT_NOTIFICATION_POLL: return L10N[L10N_EN_US][L10N_NOTIF_POLL];
    case MSTDNT_NOTIFICATION_EMOJI_REACT: return L10N[L10N_EN_US][L10N_NOTIF_REACTED_WITH];
    default: return "";
    }
}

const char* notification_type_compact_str(mstdnt_notification_t type)
{
    switch (type)
    {
    case MSTDNT_NOTIFICATION_FOLLOW: return L10N[L10N_EN_US][L10N_NOTIF_COMPACT_FOLLOW];
    case MSTDNT_NOTIFICATION_FOLLOW_REQUEST: return L10N[L10N_EN_US][L10N_NOTIF_COMPACT_FOLLOW_REQUEST];
    case MSTDNT_NOTIFICATION_REBLOG: return L10N[L10N_EN_US][L10N_NOTIF_COMPACT_REPEATED];
    case MSTDNT_NOTIFICATION_FAVOURITE: return L10N[L10N_EN_US][L10N_NOTIF_COMPACT_LIKED];
    case MSTDNT_NOTIFICATION_POLL: return L10N[L10N_EN_US][L10N_NOTIF_COMPACT_POLL];
    case MSTDNT_NOTIFICATION_EMOJI_REACT: return L10N[L10N_EN_US][L10N_NOTIF_COMPACT_REACTED_WITH];
    default: return "";
    }
}


const char* notification_type_svg(mstdnt_notification_t type)
{
    switch (type)
    {
    case MSTDNT_NOTIFICATION_FOLLOW: return data_follow_svg;
    case MSTDNT_NOTIFICATION_FOLLOW_REQUEST: return "";
    case MSTDNT_NOTIFICATION_REBLOG: return data_repeat_svg;
    case MSTDNT_NOTIFICATION_FAVOURITE: return data_like_svg;
    case MSTDNT_NOTIFICATION_POLL: return "";
    case MSTDNT_NOTIFICATION_EMOJI_REACT: return "";
    default: return "";
    }
}
