#pragma safety enable;

struct X
{
    char* _Owner _Opt p;
};

char* _Owner _Opt strdup(const char* s);

int main()
{
    struct X x  = {
        .p = strdup("a")
    };
} //lint 29 leak x.p
