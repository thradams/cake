#pragma safety enable

struct X { int i; };

void f(void)
{
    int a = *(int*)0; //lint 33
    int b = ((struct X*)0)->i; //lint 33
    int c = *(int* _Opt)nullptr; //lint 33
}
