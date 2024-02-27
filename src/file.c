void  free(void* _Owner p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner s;
};
void init(_Out struct X* px)
{
    static_state(px, "maybe-null");
    static_state(px->s, "uninitialized");
    px->s = strdup("a");
}

int main() {
    struct X x;
    init(&x);
    free(x.s);
}
void dummy() {}

#pragma cake diagnostic check "-Wmaybe-uninitialized"