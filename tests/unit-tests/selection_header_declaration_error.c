/*
  C2Y 6.8.5.1 selection-header (N3580) constraints on declaration-condition:

    - declares exactly one object and the initializer is mandatory
    - storage-class specifiers other than auto, constexpr or register are not allowed
    - the declared object is the controlling expression, so it must be scalar
    - the clause after the init-statement is an expression only (the C++
      "condition" form with a second declaration is not C2Y)
*/

struct S { int a; };

int main(void)
{
    if (int x = 1, y = 2) //lint 2050 shall declare exactly one object
    {
    }

    if (int x) //lint 2050 must have an initializer
    {
    }

    if (static int once = 1) //lint 2050 storage-class specifiers
    {
        once = 0;
    }

    if (extern int e = 1) //lint 2050 1980 storage-class specifiers
    {
    }

    if (typedef int T = 1) //lint 2050 storage-class specifiers
    {
    }

    switch (static int s = 1) //lint 2050 storage-class specifiers
    {
        default: break;
    }

    if (struct S s = {0}) //lint 940 controlling expression must have scalar type
    {
    }

    if (int x = 0; int y = x + 1) //lint 2050 a declaration is not allowed after the init-statement
    {
    }

    return 0;
}
