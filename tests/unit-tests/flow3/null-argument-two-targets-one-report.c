#pragma safety enable

void use(int* p);

void f(int c)
{
    int* a = 0;
    int* b = 0;
    int** pp = c ? &a : &b;
    use(*pp); //lint 35
}
