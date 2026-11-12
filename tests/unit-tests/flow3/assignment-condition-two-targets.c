#pragma safety enable

int* get(void);
void use(int* p);

void f(int c)
{
    int* a = 0;
    int* b = 0;
    int** pp = c ? &a : &b;
    if ((*pp = get()) != 0)
    {
        use(*pp);
    }
}
