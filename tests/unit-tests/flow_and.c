#pragma safety enable


int* _Opt f();

int main()
{
    int* _Opt p1 = f();
    int* _Opt p2 = f();

    if (p1 && p2)
    {
        static_state(p1, "not-null");
        static_state(p2, "not-null");
    }
    else
    {
    }
}