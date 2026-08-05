#pragma safety enable


int* _Opt f();

int main()
{
    int* _Opt p1 = f();
    int* _Opt p2 = f();

    if (p1 && p2)
    {
        compile_assert(p1 != 0);
        compile_assert(p2 != 0);
    }
    else
    {
    }
}
