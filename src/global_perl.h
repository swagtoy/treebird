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
#include <pthread.h>

//    hv_stores(ssn_hv, "id", newSVpv(acct->id, 0)); 
#define hvstores_str(hv, key, val) if (val) { hv_stores((hv), key, sv_2mortal(newSVpv((val), 0))); }
#define hvstores_int(hv, key, val) hv_stores((hv), key, sv_2mortal(newSViv((val))))
#define hvstores_ref(hv, key, val) if (val) {  hv_stores((hv), key, sv_2mortal(newRV_noinc((SV*)(val)))); }

/* Seeing all this shit littered in Treebird's code made me decide to write some macros */
#define PERL_STACK_INIT perl_lock(); \
    dSP;                             \
    ENTER;                           \
    SAVETMPS;                        \
    PUSHMARK(SP)

#define PERL_STACK_SCALAR_CALL(name) PUTBACK;   \
    call_pv((name), G_SCALAR);                  \
    SPAGAIN

/* you MUST assign scalar from savesharedsvpv, then free when done */
#define PERL_GET_STACK_EXIT savesvpv(POPs);   \
    PUTBACK;                                        \
    FREETMPS;                                       \
    LEAVE;                                          \
    perl_unlock()

#define PERLIFY_MULTI(type, types, mstype) AV* perlify_##types(const struct mstype* const types, size_t len) { \
        if (!(types && len)) return NULL;                               \
        AV* av = newAV();                                               \
        for (size_t i = 0; i < len; ++i)                                \
            av_push(av, sv_2mortal(newRV_noinc((SV*)sv_2mortal(perlify_##type(types + i))))); \
        return sv_2mortal(av);                                        \
    }
    


extern PerlInterpreter* my_perl;
extern HV* template_files;
extern pthread_mutex_t perllock_mutex;

#ifndef SINGLE_THREADED
#define perl_lock() do { pthread_mutex_lock(&perllock_mutex); } while (0)
#define perl_unlock() do { pthread_mutex_unlock(&perllock_mutex); } while (0)
#else
// NOOP
#define perl_lock() ;;
#define perl_unlock() ;;
#endif

#define ARG_UNDEFINED() do { mXPUSHs(&PL_sv_undef); } while (0)

void init_template_files(pTHX);
void cleanup_template_files();

#endif /* GLOBAL_PERL_H */
