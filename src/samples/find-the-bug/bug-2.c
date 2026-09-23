
#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
};

void delete(struct X * _Owner _Opt p)
{
    if (p)
    {
       /*forgot to free p->name*/
       free(p);
    }
}

int main()
{
    struct X * _Owner _Opt p = calloc(1, sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        delete(p);
    }
}
