#pragma safety  enable

struct X { char* name; };

void f(struct X x) {};

int main()
{
    struct X x;
    f(x); //lint 30 passing a possible uninitialized object 'x.name' (see line 9)
}
 