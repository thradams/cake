#pragma safety enable

#include <stdlib.h>
#include <string.h>

struct X {
  char *_Owner name;
  char *_Owner surname;
};

void delete(struct X * _Owner _Nullable p)
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
    struct X * _Owner p = malloc(sizeof * p);
    if (p)
    {
        p->name = strdup("a");
        /*p->surname is uninitialized*/
        delete(p);
    }
}
