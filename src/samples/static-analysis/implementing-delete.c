#pragma safety enable 

_Clear void * _Owner calloc(unsigned long n, unsigned long s);
char * _Owner strdup(const char* s);
void free(void * _Nullable _Owner p);

struct X {
    char * _Owner text;
};

void x_delete(struct X * _Owner _Nullable p)
{
    if (p)
    {
        free(p->text);
        free(p);
    }
}

int main() {
    struct X * _Owner p = calloc(1, sizeof * p);
    if (p){
        p->text = strdup("a");
    }
    x_delete(p);

}
