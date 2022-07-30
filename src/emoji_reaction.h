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

#ifndef EMOJI_REACTION_H
#define EMOJI_REACTION_H
#include <mastodont.h>
#include "global_perl.h"

char* construct_emoji_reaction(char* id, struct mstdnt_emoji_reaction* emo, size_t* str_len);
char* construct_emoji_reactions(char* id, struct mstdnt_emoji_reaction* emos, size_t emos_len, size_t* str_len);

// Perl
HV* perlify_emoji_reaction(struct mstdnt_emoji_reaction* const emoji);
AV* perlify_emoji_reactions(struct mstdnt_emoji_reaction* const emos, size_t len);

#endif // EMOJI_REACTION_H
