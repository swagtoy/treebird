/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
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

char* construct_emoji(struct emoji_info* emoji, char* status_id, size_t* size);
int content_emoji_picker(PATH_ARGS);
char* construct_emoji_picker(char* status_id, size_t* size);

// Perl
HV* perlify_emoji(const struct mstdnt_emoji* const emoji);
AV* perlify_emojis(const struct mstdnt_emoji* const emos, size_t len);

#endif // EMOJI_H
