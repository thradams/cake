/* a non-const void* parameter says it may write (memset); one that only reads takes const void* */

typedef unsigned long size_t;
void* _Nonnull memset(void* p, int c, size_t n);
void* _Nonnull memcpy(void* dst, const void* src, size_t n);

struct buffer { char* _Nonnull data; };

void clear(const struct buffer* b)
{
    memset(b->data, 0, 10); //lint 120
}

void copy_from(const struct buffer* b, char* out)
{
    memcpy(out, b->data, 10);
}