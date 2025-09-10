#pragma safety enable;

struct X
{
    char* _Owner _Opt p;
};

char* _Owner _Opt strdup(const char* s);

int main()
{
    struct X x [[cake::leak]] = {
        .p = strdup("a")
    };
}
