#pragma safety enable


_Uninitialized void* _Owner _Opt malloc(unsigned long size);

struct X {
    char* _Owner text;
};

void x_delete(struct X* _Owner  _Opt p);

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof(struct X));
    //warning: uninitialized object 'p.text'
    
    x_delete(p); //lint 30 passing a possible uninitialized object '(*p)' -- malloc is _Uninitialized
}
