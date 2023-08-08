#pragma once

#ifdef EXPORT_MYSQL_UDF

#include <mysql.h>

extern "C" {
my_bool querystring_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char   *querystring(UDF_INIT *initid, UDF_ARGS *args, char *result,
                    unsigned long *length, char *is_null, char *error);
void    querystring_deinit(UDF_INIT *initid);
}

#endif

extern "C" {
char *querystring_dec(const char *val);
char *querystring_get(const char *qs, const char *key);
}