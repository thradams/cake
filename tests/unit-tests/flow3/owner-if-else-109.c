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
        /* No discard warning: malloc is declared _Uninitialized above, so
           p->name is uninitialized storage here, not an owner holding a
           resource that would need releasing first. */
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
