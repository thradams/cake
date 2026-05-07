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

    free(p->name); //lint 33 30

    // warning: object 'name' may not be empty

    free(p); //lint 29
}
