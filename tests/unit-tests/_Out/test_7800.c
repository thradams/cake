#pragma safety enable


void* /*_Owner*/ /*_Opt*/ calloc(int n, int size);

struct X {
    char* /*_Owner*/ /*_Opt*/ name;
};

void* /*_Owner*/ /*_Opt*/ f1()
{
    struct X* /*_Owner*/ /*_Opt*/ p = calloc(1, sizeof(struct X));
    if (p)
    {
        p->name = calloc(1,2);
    }    
    return p;    
}
#pragma cake diagnostic check "-Wmissing-destructor"
