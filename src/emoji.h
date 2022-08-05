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

#ifndef EMOJI_H
#define EMOJI_H
#include <stddef.h>
#include <mastodont.h>
#include "global_perl.h"
#include "emoji_codes.h"
#include "path.h"

#define EMOJI_FACTOR_NUM 32

// No use?
enum emoji_picker_cat
{
    EMOJI_CAT_CUSTOM,
    EMOJI_CAT_FACES,
};

char* emojify(char* content, struct mstdnt_emoji* emos, size_t emos_len);
char* construct_emoji(struct emoji_info* emoji, char* status_id, size_t* size);
void content_emoji_picker(PATH_ARGS);
char* construct_emoji_picker(char* status_id, size_t* size);

// Perl
HV* perlify_emoji(struct mstdnt_emoji* const emoji);
AV* perlify_emojis(struct mstdnt_emoji* const emos, size_t len);

#endif // EMOJI_H
