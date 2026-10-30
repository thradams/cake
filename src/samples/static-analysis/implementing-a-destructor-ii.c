#pragma safety enable

char * _Owner strdup(const char* s);
void free(void * _Nullable _Owner p);

struct X {
    char *_Owner name;
};

void x_destroy(_Dtor struct X * x)
{
    free(x->name);
}

int main() {
    struct X x = {0};
    x.name = strdup("a");
    x_destroy(&x);
}
