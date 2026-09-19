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
    /* FIXED: the _Out call keeps the non-_Opt _Owner member x.s non-null, no "possible null pointer" */
    free(x.s);
}
