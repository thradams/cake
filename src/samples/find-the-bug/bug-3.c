#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
  char *_Owner _Opt surname;
};

void delete(struct X * _Owner _Opt p)
{
    if (p)
    {
       free(p->name);
       /*forgot to free p->surname*/
       free(p);
    }
}

int main()
{
    struct X * _Owner _Opt p = malloc(sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        /*p->surname is uninitialized*/
        delete(p);
    }
}
