#pragma safety enable


_Uninitialized void* _Owner malloc(unsigned long size);

struct X {
    char* _Nonnull _Owner text;
};

void x_delete(struct X* _Owner  _Opt p);

int main()
{
    struct X* _Owner p = malloc(sizeof(struct X));
    //warning: uninitialized object 'p.text'
    
    x_delete(p); //lint 30 passing a possible uninitialized object '(*p)' -- malloc is _Uninitialized
}
