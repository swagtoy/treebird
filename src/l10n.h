/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef TREEBIRD_L10N_H
#define TREEBIRD_L10N_H

enum l10n_locale
{
    L10N_EN_US = 0,
    L10N_ES_ES,
    L10N_CH_TW,
    L10N_LOCALE_LEN,
};

#define l10n_normalize(index) ((index) < L10N_LOCALE_LEN && (index) >= 0 ? (index) : 0)

enum l10n_string
{
    /* GENERAL */
    L10N_APP_NAME = 0,
    L10N_HOME,
    L10N_LOCAL,
    L10N_FEDERATED,
    L10N_NOTIFICATIONS,
    L10N_LISTS,
    L10N_FAVOURITES,
    L10N_BOOKMARKS,
    L10N_DIRECT,
    L10N_CONFIG,
    L10N_SEARCH_PLACEHOLDER,
    L10N_SEARCH_BUTTON,

    /* CONFIG */
    L10N_GENERAL,
    L10N_ACCOUNT,
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

    /* ACCOUNT */
    L10N_ACCT_MENU,
    L10N_SUBSCRIBE,
    L10N_UNSUBSCRIBE,
    L10N_BLOCK,
    L10N_UNBLOCK,
    L10N_MUTE,
    L10N_UNMUTE,
    L10N_TAB_STATUSES,
    L10N_TAB_FOLLOWING,
    L10N_TAB_FOLLOWERS,
    L10N_TAB_SCROBBLES,
    L10N_TAB_MEDIA,
    L10N_TAB_PINNED,
    L10N_FOLLOWS_YOU,
    L10N_FOLLOW,
    L10N_FOLLOW_PENDING,
    L10N_FOLLOWING,
    L10N_BLOCKED,

    /* STATUS */
    L10N_REPLY,
    L10N_REPEAT,
    L10N_LIKE,
    L10N_QUICK,
    L10N_VIEW,
    L10N_IN_REPLY_TO,

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
    L10N_VIS_LIST,

    /* LOGIN */
    L10N_LOGIN,
    L10N_REGISTER,
    L10N_USERNAME,
    L10N_PASSWORD,
    L10N_LOGIN_BTN,
    L10N_LOGIN_HEADER,
    L10N_LOGIN_FAIL,

    /* NOTIFICATIONS */
    L10N_NOTIF_LIKED,
    L10N_NOTIF_REACTED_WITH,
    L10N_NOTIF_REPEATED,
    L10N_NOTIF_FOLLOW,
    L10N_NOTIF_FOLLOW_REQUEST,
    L10N_NOTIF_POLL,

    L10N_NOTIF_COMPACT_LIKED,
    L10N_NOTIF_COMPACT_REACTED_WITH,
    L10N_NOTIF_COMPACT_REPEATED,
    L10N_NOTIF_COMPACT_FOLLOW,
    L10N_NOTIF_COMPACT_FOLLOW_REQUEST,
    L10N_NOTIF_COMPACT_POLL,

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
        "Favorites",
        "Bookmarks",
        "Direct",
        "Config",
        "Search",
        "Search",

        /* CONFIG */
        "General",
        "Account",
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

        /* ACCOUNT */
        "Menu",
        "Subscribe",
        "Unsubscribe",
        "Block",
        "Unblock",
        "Mute",
        "Unmute",
        "Statuses",
        "Following",
        "Followers",
        "Scrobbles",
        "Media",
        "Pinned",
        "Follows you!",
        "Follow",
        "Follow pending",
        "Following!",
        "You are blocked by this user.",

        /* STATUS */
        "Reply",
        "Repeat",
        "Like",
        "Quick",
        "View",
        "In reply to",

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
        "List",

        /* LOGIN */
        "Login",
        "Register",
        "Username",
        "Password",
        "Login",
        "Login / Register",
        "Couldn't login",

        /* NOTIFICATIONS */
        "liked your status",
        "reacted with",
        "repeated your status",
        "followed you",
        "wants to follow you",
        "poll results",

        "liked",
        "reacted",
        "repeated",
        "followed",
        "follows?",
        "poll",
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
        "Favourites",
        "Bookmarks",
        "Directo",
        "Configuración",
        "Búsqueda",
        "Búsqueda",

        /* CONFIG */
        "General",
        "Account",
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

        /* ACCOUNT */
        "Menu",
        "Subscribe",
        "Unsubscribe",
        "Block",
        "Unblock",
        "Mute",
        "Unmute",
        "Statuses",
        "Following",
        "Followers",
        "Scrobbles",
        "Media",
        "Pinned",
        "Follows you!",
        "Follow",
        "Follow pending",
        "Following!",
        "You are blocked by this user.",

        /* STATUS */
        "Respuesta",
        "Impulso",
        "Me gusta",
        "Rápido",
        "View",
        "In reply to",
        
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
        "List",

        /* LOGIN */
        "Login",
        "Register",
        "Username",
        "Password",
        "Login",
        "Login / Register",
        "Couldn't login",
        
        /* NOTIFICATIONS */
        "liked your status",
        "reacted with",
        "repeated your status",
        "followed you",
        "wants to follow you",
        "poll results",

        "liked",
        "reacted",
        "repeated",
        "followed",
        "follows?",
        "poll",
    },

    // CH_TW
    {
        /* GENERAL */
        "Treebird", // L10N_APP_NAME
        "首頁",
        "本地",
        "聯邦",
        "通知",
        "清單",
        "我的最愛",
        "書籤",
        "私訊",
        "設定",
        "搜尋",
        "搜尋",

        /* CONFIG */
        "一般",
        "帳號",
        "JavaScript",
        "快速動作 - 在背景執行讚、助推等動作",
        "快速回覆 - 回覆不需重定向",
        "即時動態 - 即時獲取動態，不需重新載入",
        "外觀",
        "主題選擇",
        "Treebird 2.0 - 預設，簡約主題",
        "Treebird 3.0 - 扁平化，摩登主題",
        "配色方案",
        "淺色",
        "深色",
        "儲存",

        /* ACCOUNT */
        "選單",
        "訂閱",
        "取消訂閱",
        "封鎖",
        "解除封鎖",
        "靜音",
        "解除靜音",
        "狀態",
        "追隨中",
        "追隨者",
        "塗鴉",
        "媒體",
        "置頂",
        "在追隨你！",
        "追隨",
        "等待批准追隨請求",
        "追隨中!",
        "你被這個使用者封鎖了。",

        /* STATUS */
        "回覆",
        "助推",
        "讚",
        "快速",
        "檢視",
        "回覆",

        /* ERRORS */
        "找不到內容",
        "找不到狀態",
        "找不到帳號",

        /* VISIBILITIES */
        "公開",
        "首頁",
        "私密",
        "私訊",
        "本地",
        "List",

        /* LOGIN */
        "登入",
        "註冊",
        "使用者名稱",
        "密碼",
        "登入",
        "登入 / 註冊",
        "無法登入",

        /* NOTIFICATIONS */
        "讚了你的動態",
        "反應了",
        "助推了你的動態",
        "追隨了你",
        "想要追隨你",
        "投票結果",

        "讚了",
        "反應了",
        "助推了",
        "跟隨了",
        "跟隨嗎？",
        "投票",
    }
};

#endif // TREEBIRD_L10N_H
