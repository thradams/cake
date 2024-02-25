//en.cppreference.com/w/c/program/unreachable.html
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
 
struct Color { uint8_t r, g, b, a; };
struct ColorSpan { struct Color* data; size_t size; };
 
// Assume that only restricted set of texture caps is supported.
struct ColorSpan allocate_texture(size_t xy)
{
    switch (xy)
    {
    case 128: [[fallthrough]];
    case 256: [[fallthrough]];
    case 512:
    {
        /* ... */
        struct ColorSpan result = {
            .data = malloc(xy * xy * sizeof(struct Color)),
            .size = xy * xy
        };
        if (!result.data)
            result.size = 0;
        return result;
    }
    default:
        unreachable();
    }
}
 
int main(void)
{
    struct ColorSpan tex = allocate_texture(128); // OK
    assert(tex.size == 128 * 128);
 
    struct ColorSpan badtex = allocate_texture(32);  // Undefined behavior
 
    free(badtex.data);
    free(tex.data);
}