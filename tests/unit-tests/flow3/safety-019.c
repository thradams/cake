#pragma safety enable


int* f();

int main()
{
    int* p1 = f();
    int* p2 = f();

    if (p1 && p2)
    {
        compile_assert(p1 != 0);
        compile_assert(p2 != 0);
    }
    else
    {
    }
}
