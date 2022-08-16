#include "hash.h"
#include <string.h>

unsigned int stringhash(const char* key)
{
    // hash key to unsigned int value by pseudorandomizing transform
    // (algorithm copied from STL char hash in xfunctional)
    unsigned int uHashVal = 2166136261U;
    unsigned int uFirst = 0;
    unsigned int uLast = (unsigned int)strlen(key);
    unsigned int uStride = 1 + uLast / 10;

    for (; uFirst < uLast; uFirst += uStride)
    {
        uHashVal = 16777619U * uHashVal ^ (unsigned int)key[uFirst];
    }

    return (uHashVal);
}