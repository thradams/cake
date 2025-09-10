#pragma safety enable


void* _Owner _Opt malloc(int i);
void free(void* _Owner _Opt);

struct X {
    char* _Owner _Opt name;
};

int main()
{
    struct X* _Owner _Opt p = malloc(sizeof(struct X));
    if (p)
    {
        p->name = malloc(1);
    }
    else
    {
        //p->name = malloc(1);        
    }
    //warning: pointer may be null
    [[cake::w30]] [[cake::w33]]
        free(p->name);

    // warning: object 'name' may not be empty
    [[cake::w29]]
    free(p);
}
