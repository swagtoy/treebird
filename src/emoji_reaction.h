/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef EMOJI_REACTION_H
#define EMOJI_REACTION_H
#include <mastodont.h>
#include "global_perl.h"

// Perl
HV* perlify_emoji_reaction(const struct mstdnt_emoji_reaction* const emoji);
AV* perlify_emoji_reactions(const struct mstdnt_emoji_reaction* const emos, size_t len);

#endif // EMOJI_REACTION_H
