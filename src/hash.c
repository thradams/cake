
#pragma safety enable


#include "hash.h"
#include <string.h>

unsigned int string_hash(const char* key)
{
    // hash key to unsigned int value by pseudorandomizing transform
    // (algorithm copied from STL char hash in xfunctional)
    unsigned int hash_val = 2166136261U;
    unsigned int first = 0;
    unsigned int last = (unsigned int)strlen(key);
    unsigned int stride = 1 + last / 10;

    for (; first < last; first += stride)
    {
        hash_val = 16777619U * hash_val ^ (unsigned int)key[first];
    }

    return (hash_val);
}
