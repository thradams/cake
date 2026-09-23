#pragma safety enable 

void * _Opt _Owner _Clear calloc(unsigned long n, unsigned long s);
char * _Opt _Owner strdup(const char* s);
void free(void * _Opt _Owner p);

struct X {
    char * _Owner _Opt text;
};

void x_delete(struct X * _Owner _Opt p)
{
    if (p)
    {
        free(p->text);
        free(p);
    }
}

int main() {
    struct X * _Owner _Opt p = calloc(1, sizeof * p);
    if (p){
        p->text = strdup("a");
    }
    x_delete(p);

}
