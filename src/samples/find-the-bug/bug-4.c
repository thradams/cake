
#pragma safety enable

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
  char *_Owner _Opt name;
  char *_Owner _Opt surname;
};

void change(struct X * p)
{
     free(p->name);       
}

int main()
{
    struct X x = {0};
    x.name = strdup("a");
    change(&x);
    printf("%s", x.name);
    /*forgot to free x.name and x.surname*/
}
