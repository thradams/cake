#pragma safety enable



void */*_Owner*/ calloc(int n , int i);
void free(void  * /*_Owner*/ /*_Opt*/ p);

struct X{
  char * /*_Owner*/ /*_Opt*/ name;
};

int main()
{
    struct X * /*_Owner*/ p = calloc(1, sizeof(struct X));
    if (p)
    {
        p->name = calloc(1, 2);
    }

    
    if (p)
      free(p->name);

    free(p);
}
