#include "parse_url.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern "C" {

#ifndef strmov
#define strmov(A, B) stpcpy((A), (B))
#endif

char *querystring_dec(const char *val) {
    if (val == nullptr) {
        return nullptr;
    }

    char  x, y;
    char *decoded = (char *)malloc(strlen(val) + 1);
    char *ptr     = decoded;
    while (*val) {
        if ((*val == 37) && //%
            ((x = val[1]) && (y = val[2])) && (isxdigit(x) && isxdigit(y))) {
            if (x >= 97) {
                x -= 32; // 97,65
            }
            if (x >= 65) {
                x -= 55; // 65
            } else {
                x -= 48;
            }
            if (y >= 97) {
                y -= 32; // 97,65
            }
            if (y >= 65) {
                y -= 55; // 65
            } else {
                y -= 48;
            }
            *ptr++ = 16 * x + y;
            val += 3;
        } else if (*val == 43) {
            *ptr++ = 32;
            val++;
        } else {
            *ptr++ = *val++;
        }
    }
    *ptr++ = '\0';
    return decoded;
}

char *querystring_get(const char *qs, const char *key) {
    enum state_t {
        state_begin,
        state_key,
        state_eq,
        state_value,
        state_fragment,
        state_end
    };

    int         ik    = 0;
    int         iv    = 0;
    int         vb    = 0;
    int         ve    = 0;
    bool        done  = false;
    const char *p     = qs;
    state_t     state = state_begin;
    for (int i = 0; i < strlen(qs) && !done; i++) {
        switch (state) {
        case state_begin:
            ik = 0;
            if (i == 0 || p[i] == '=' || p[i] == '&' || p[i] == '?') {
                state = state_key;
            }
            break;
        case state_key:
            if (p[i] == '=' || p[i] == '&') {
                ik = 0;
                continue;
            }
            if (p[i] != key[ik]) {
                state = state_begin;
                continue;
            }
            ik++;
            if (ik == strlen(key)) {
                state = state_eq;
                continue;
            }
            break;
        case state_eq:
            if (p[i] == '=') {
                vb    = i + 1;
                ve    = 0;
                state = state_value;
            } else {
                state = state_begin;
            }
            break;
        case state_value:
            ve = i + 1;
            if (p[i] == '&') {
                ve   = i;
                done = true;
                continue;
            }
            iv++;
            break;
        case state_fragment:
            break;
        case state_end:
            break;
        }
    }

    if (ve > vb) {
        char *r = (char *)malloc(ve - vb + 1);
        memset(r, 0, (ve - vb + 1));
        strncpy(r, qs + vb, ve - vb);
        return r;
    }

    return nullptr;
}

#ifdef EXPORT_MYSQL_UDF

my_bool querystring_init(UDF_INIT *initid, UDF_ARGS *args, char *message) {
    if (args->arg_count != 2) {
        strmov(message, "This function takes 2 arguments");
        return 1;
    }

    for (uint i = 0; i < args->arg_count; i++)
        args->arg_type[i] = STRING_RESULT;

    // Fool MySQL to think that this function is a constant
    // This will ensure that MySQL only evalutes the function
    // when the rows are sent to the client and not before any ORDER BY
    // clauses
    initid->const_item = 1;
    initid->maybe_null = 1;
    initid->ptr        = nullptr;
    return 0;
}

char *querystring_url(UDF_INIT *initid, UDF_ARGS *args, char *result,
                      unsigned long *length, char *is_null, char *error) {
    if (args->args[0] == nullptr || args->args[1] == nullptr) {
        *is_null = 1;
        return 0;
    }

    char *val = querystring_get(args->args[0], args->args[1]);
    if (val == nullptr) {
        *is_null = 1;
        return 0;
    }

    char *ptr   = querystring_dec(val);
    initid->ptr = ptr;
    *length     = strlen(ptr);

    free(val);
    return ptr;
}

void querystring_deinit(UDF_INIT *initid) {
    if (initid->ptr != nullptr) {
        free(initid->ptr);
        initid->ptr = nullptr;
    }
}

#endif
}