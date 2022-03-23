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

#ifndef TREEBIRD_L10N_H
#define TREEBIRD_L10N_H

enum l10n_locale
{
    L10N_EN_US = 0,
    L10N_ES_ES,
};

enum l10n_string
{
    /* GENERAL */
    L10N_APP_NAME = 0,
    L10N_HOME,
    L10N_LOCAL,
    L10N_FEDERATED,
    L10N_NOTIFICATIONS,
    L10N_LISTS,
    L10N_DIRECT,
    L10N_CONFIG,
    L10N_SEARCH_PLACEHOLDER,
    L10N_SEARCH_BUTTON,

    /* CONFIG */
    L10N_GENERAL,
    L10N_JAVASCRIPT,
    L10N_CFG_QUICK_ACTIONS,
    L10N_CFG_QUICK_REPLY,
    L10N_CFG_LIVE_STATUSES,
    L10N_APPEARANCE,
    L10N_THEME_VARIANT,
    L10N_THEME_TREEBIRD20,
    L10N_THEME_TREEBIRD30,
    L10N_COLOR_SCHEME,
    L10N_LIGHT,
    L10N_DARK,
    L10N_SAVE,

    /* STATUS */
    L10N_REPLY,
    L10N_REPEAT,
    L10N_LIKE,
    L10N_QUICK,
    L10N_VIEW,

    /* ERRORS */
    L10N_PAGE_NOT_FOUND,
    L10N_STATUS_NOT_FOUND,
    L10N_ACCOUNT_NOT_FOUND,

    /* VISIBILITIES */
    L10N_VIS_PUBLIC,
    L10N_VIS_UNLISTED,
    L10N_VIS_PRIVATE,
    L10N_VIS_DIRECT,
    L10N_VIS_LOCAL,

    /* LOGIN */
    L10N_LOGIN_HEADER,
    L10N_LOGIN_FAIL,

    /* NOTIFICATIONS */
    L10N_NOTIF_LIKED,
    L10N_NOTIF_REACTED_WITH,
    L10N_NOTIF_REPEATED,
    L10N_NOTIF_FOLLOW,
    L10N_NOTIF_FOLLOW_REQUEST,

    L10N_NOTIF_COMPACT_LIKED,
    L10N_NOTIF_COMPACT_REACTED_WITH,
    L10N_NOTIF_COMPACT_REPEATED,
    L10N_NOTIF_COMPACT_FOLLOW,
    L10N_NOTIF_COMPACT_FOLLOW_REQUEST,

    _L10N_LEN,
};

static const char* const L10N[][_L10N_LEN] = {
    // EN_US
    {
        /* GENERAL */
        "Treebird", // L10N_APP_NAME
        "Home",
        "Local",
        "Federated",
        "Notifications",
        "Lists",
        "Direct",
        "Config",
        "Search",
        "Search",

        /* CONFIG */
        "General",
        "JavaScript",
        "Quick actions - Likes, Boosts, etc done in background",
        "Quick reply - Replies don't require redirects",
        "Live statuses - Statuses fetch on the fly",
        "Appearance",
        "Theme variant",
        "Treebird 2.0 - Default, simple theme",
        "Treebird 3.0 - Flat, modern theme",
        "Color Scheme",
        "Light",
        "Dark",
        "Save",

        /* STATUS */
        "Reply",
        "Repeat",
        "Like",
        "Quick",
        "View",

        /* ERRORS */
        "Content not found",
        "Status not found",
        "Account not found",

        /* VISIBILITIES */
        "Public",
        "Unlisted",
        "Private",
        "Direct",
        "Local",

        /* LOGIN */
        "Login / Register",
        "Couldn't login",

        /* NOTIFICATIONS */
        "liked your status",
        "reacted with",
        "repeated your status",
        "followed you",
        "wants to follow you",

        "liked",
        "reacted",
        "repeated",
        "followed",
        "follows?"
    },

    // ES_ES
    {
        /* GENERAL */
        "Treebird", // L10N_APP_NAME
        "Inicio",
        "Local",
        "Federado",
        "Notificaciones",
        "Listas",
        "Directo",
        "Configuración",
        "Búsqueda",
        "Búsqueda",

        /* CONFIG */
        "General",
        "JavaScript",
        "Acciones rápidas - Me gusta, Impulsos, etc. se hacen en segundo plano",
        "Respuesta rápidas - Las respuestas no requieren redirección",
        "Estados en vivo - Los estados se obtienen sobre la marcha",
        "Apariencia",
        "Variante de tema",
        "Treebird 2.0 - Tema simple predeterminado",
        "Treebird 3.0 - Tema plano y moderno",
        "Esquema de colores",
        "Claro",
        "Oscuro",
        "Guardar",

        /* STATUS */
        "Respuesta",
        "Impulso",
        "Me gusta",
        "Rápido",
        "View (Translate me)",
        
        /* ERRORS */
        "Content not found",
        "Status not found",
        "Account not found",
        
        /* VISIBILITIES */
        "Public",
        "Unlisted",
        "Private",
        "Direct",
        "Local",

        /* LOGIN */
        "Login / Register",
        "Couldn't login",
        
        /* NOTIFICATIONS */
        "liked your status",
        "reacted with",
        "repeated your status",
        "followed you",
        "wants to follow you",

        "liked",
        "reacted",
        "repeated",
        "followed",
        "follows?",
    },
};

#endif // TREEBIRD_L10N_H
