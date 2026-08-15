#pragma safety enable


void  free(void* _Owner p);
char* _Owner strdup(const char* s);

struct X {
    char* _Owner s;
};
void init(_Out struct X* px)
{
    compile_assert(px != 0);
    //assert_state(px->s, "uninitialized");
    px->s = strdup("a");
}

int main() {
    struct X x;
    init(&x);
    /* FIXED: this used to warn "passing a possible null pointer '.s'"
       here -- init's _Out call invalidated x.s to a generic ANY value
       (could be zero), even though x.s is a plain (non-_Opt) _Owner
       pointer whose type already guarantees it's never null. */
    free(x.s);
}
