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

#ifndef GLOBAL_PERL_H
#define GLOBAL_PERL_H
#include <EXTERN.h>
#include <perl.h>

//    hv_stores(ssn_hv, "id", newSVpv(acct->id, 0)); 
#define hvstores_str(hv, key, val) hv_stores((hv), key, ((val) ? newSVpv((val), 0) : &PL_sv_undef))
#define hvstores_int(hv, key, val) hv_stores((hv), key, newSViv((val)))
#define hvstores_ref(hv, key, val) hv_stores((hv), key,                 \
                                             ((val) ? newRV_inc((SV* const)(val)) : &PL_sv_undef))

static PerlInterpreter* perl;

#endif /* GLOBAL_PERL_H */
