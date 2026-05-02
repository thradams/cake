#pragma safety enable


void  free(void* _Owner p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner s;
};
void init(_Ctor struct X* px)
{
    assert_state(px, "not-null");
    assert_state(px->s, "uninitialized");
    px->s = strdup("a");
}

int main() {
    struct X x;
    init(&x);
    free(x.s);
}
