#include "parse_url.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *inputs[] = {
        "/abc?key1=val1&key2=val2&key3=val3",
        "/key1/key2/key3?key1=a+b&key2=val2",
        "/abc?key1=m&key2=n&key3=x%20y",
        "/abc?key1=val1&key2=val2&key3=val3",
    };

    const char *keys[] = {
        "key1",
        "key1",
        "key3",
        "key4",
    };

    const char *wanted[] = {
        "val1",
        "a b",
        "x y",
        nullptr,
    };

    for (int i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
        char *p = querystring_get(inputs[i], keys[i]);
        char *v = querystring_dec(p);
        if (v == nullptr && wanted[i] != nullptr) {
            printf("unexpected for url %s, key %s, wanted: %s, got null\n",
                   inputs[i], keys[i], wanted[i]);
        } else if (v != nullptr && wanted[i] == nullptr) {
            printf("unexpected for url %s, key %s, wanted: nullptr, got %s\n",
                   inputs[i], keys[i], v);
        } else if (strcmp(v, wanted[i]) != 0) {
            printf("unexpected for url %s, key %s, wanted: %s, got %s\n",
                   inputs[i], keys[i], wanted[i], v);
        } else {
            printf("test case %d done\n", i + 1);
        }
    }

    printf("completed\n");
}