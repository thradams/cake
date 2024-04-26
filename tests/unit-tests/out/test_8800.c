#pragma safety enable


void */*_Owner*/ /*_Opt*/ malloc(int i);
void free(void  */*_Owner*/ /*_Opt*/);

struct X{
  char * /*_Owner*/ /*_Opt*/ name;
};

int main()
{
    struct X * /*_Owner*/ /*_Opt*/ p = malloc(sizeof(struct X));
    if (p)
    {
        p->name = malloc(1);
    }
    else
    {        
        //p->name = malloc(1);
        //#pragma cake diagnostic check "-Wanalyzer-null-dereference"
    }
    free(p->name);
#pragma cake diagnostic check "-Wanalyzer-null-dereference"
    free(p);
}
