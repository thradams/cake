void  free(void* _Owner p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner s;
};

void init(_Out struct X *  px)
{
    static_state(px, "maybe-null");
    static_state(px->s, "uninitialized");
    px->s = strdup("a");
}

void set(struct X* px, const char* text)
{
    static_state(px, "maybe-null");
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