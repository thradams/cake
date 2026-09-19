#pragma safety enable


void* _Owner _Opt _Uninitialized malloc(int i);
void free(void* _Owner _Opt);

struct X {
    char* _Owner _Opt name;
};

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof(struct X));
    if (p)
    {
        /* no discard warning: malloc is _Uninitialized, p->name is uninitialized storage, not a resource */
        p->name = malloc(1);
    }
    else
    {
        //p->name = malloc(1);        
    }
    //warning: pointer may be null

    free(p->name); //lint 33 30 -> operator applied to a null pointer; '.name' possibly uninitialized

    // warning: object 'name' may not be empty

    free(p);
}
