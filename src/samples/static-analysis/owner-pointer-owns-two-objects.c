#pragma safety enable

void * _Owner _Opt calloc(unsigned long i, unsigned long sz);
char * _Owner _Opt strdup(const char* );
void free(void * _Owner _Opt p);

struct X {
    char *_Owner _Opt name;
};

int main()
{
    struct X * _Owner _Opt p = calloc(1, sizeof * p);
    if (p) {

        p->name = strdup("hi");
        struct X x = {0};
        x = *p;
        free(x.name);

        free(p);
    }
}
