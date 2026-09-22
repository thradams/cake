/*
   C23 #embed (6.10.3) and __has_embed (6.10.1).

   embed_resource.txt holds the 13 bytes "Hello, embed!" (no newline) and
   embed_empty.txt is empty. Checks the byte sequence itself, the
   limit / prefix / suffix / if_empty parameters, that prefix/suffix are
   dropped when the resource is empty (also via limit(0)), that if_empty is
   dropped when it is not, macro expansion inside the parameters, and the
   three __has_embed results.
*/

#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

static const unsigned char all[] = {
#embed "embed_resource.txt"
};

static const unsigned char first_five[] = {
#embed "embed_resource.txt" limit(5)
};

#define LIMIT 2 + 1
#define PREFIX 0xAA,
static const unsigned char decorated[] = {
#embed "embed_resource.txt" __limit__(LIMIT) prefix(PREFIX) suffix(, 0xBB)
};

static const unsigned char limited_to_zero[] = {
#embed "embed_resource.txt" limit(0) prefix(1,) suffix(,2) if_empty(7, 8)
};

static const unsigned char empty_resource[] = {
#embed "embed_empty.txt" prefix(1,) suffix(,2) if_empty(9)
};

static const unsigned char not_empty[] = {
#embed "embed_resource.txt" limit(1) if_empty(9, 9, 9)
};

static const char as_string[] = {
#embed "embed_resource.txt" suffix(, '\0')
};

#if __has_embed("embed_resource.txt") != __STDC_EMBED_FOUND__
#error __has_embed should find embed_resource.txt
#endif

#if __has_embed("embed_empty.txt") != __STDC_EMBED_EMPTY__
#error __has_embed should report embed_empty.txt as empty
#endif

#if __has_embed("embed_resource.txt" limit(0)) != __STDC_EMBED_EMPTY__
#error __has_embed with limit(0) should report empty
#endif

#if __has_embed("this_file_does_not_exist.txt") != __STDC_EMBED_NOT_FOUND__
#error __has_embed should not find a missing resource
#endif

#if __has_embed("embed_resource.txt" vendor::param(1)) != __STDC_EMBED_NOT_FOUND__
#error __has_embed with an unsupported parameter should be 0
#endif

#if __has_embed("embed_resource.txt" limit(3) prefix(1) suffix(2) if_empty(3)) != __STDC_EMBED_FOUND__
#error __has_embed with all the standard parameters should be found
#endif

int main(void)
{
    size_t n;

    n = sizeof(all);
    CHECK(n == 13);
    CHECK(memcmp(all, "Hello, embed!", 13) == 0);

    n = sizeof(first_five);
    CHECK(n == 5);
    CHECK(memcmp(first_five, "Hello", 5) == 0);

    n = sizeof(decorated);
    CHECK(n == 5);
    CHECK(decorated[0] == 0xAA);
    CHECK(memcmp(decorated + 1, "Hel", 3) == 0);
    CHECK(decorated[4] == 0xBB);

    n = sizeof(limited_to_zero);
    CHECK(n == 2);
    CHECK(limited_to_zero[0] == 7 && limited_to_zero[1] == 8);

    n = sizeof(empty_resource);
    CHECK(n == 1);
    CHECK(empty_resource[0] == 9);

    n = sizeof(not_empty);
    CHECK(n == 1);
    CHECK(not_empty[0] == 'H');

    CHECK(strcmp(as_string, "Hello, embed!") == 0);

    return failures;
}
