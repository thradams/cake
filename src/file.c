

#include <ownership.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct X {
  char *owner name;
  char *owner surname;
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
}


