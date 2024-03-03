struct {
    int : 0;
    long a;
} b;
void c() { b.a; }
static_assert(sizeof(b.a) == sizeof(long));