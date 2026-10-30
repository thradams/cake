#pragma safety enable

void * _Owner calloc(unsigned long i, unsigned long sz);
char * _Owner strdup(const char* );
void free(void * _Owner _Nullable p);

struct X {
    char *_Owner name;
};

int main()
{
    struct X * _Owner p = calloc(1, sizeof * p);
    if (p) {

        p->name = strdup("hi");
        struct X x = {0};
        x = *p;
        free(x.name);

        free(p);
    }
}
