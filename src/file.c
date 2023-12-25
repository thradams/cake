void  free(void* _Owner _Opt p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner s;
};

void init(_Out struct X *  _Opt px)
{
    static_debug(px);
    static_state(px, "maybe-null");
    static_state(px->s, "uninitialized");
    px->s = strdup("a");
}

void set(struct X* px, const char* text)
{
    static_debug(px);
    static_state(px, "not-null");
    static_state(px->s, "maybe-null");
    free(px->s);
    px->s = strdup(text);
}

int main() {
    struct X x;
    init(&x);
    set(&x, "b");
    free(x.s);
}