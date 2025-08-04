enum E { A, B, C, D, E, F };

void f(enum E e)
{
    switch (e)
    {

    case A:
        break;
    case B:
        break;
    default:
        static_assert(_Countof(e) == 6);
    }
}

int main() {}