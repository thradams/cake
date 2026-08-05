#pragma safety enable;

struct X
{
    char* _Owner _Opt p;
};

char* _Owner _Opt strdup(const char* s);

int main()
{
    struct X x  = { // owner object (.p) not moved (root see line 12)
        .p = strdup("a")
    };
} //lint 29 owner object (.p) not moved (call-opt-nonnull see line 13)
