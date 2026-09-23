int main(void)
{
    _Bool b0 = false;
    _Bool b1 = true;
    _Bool b2 = 123;

    b1 = b0;
    b2 = 1234;
    return 0;
}

void f1(_Bool b, int i){}

_Bool f2(int i, int j)
{
    _Bool b = i;

    f1(i, i);
    f1(i = j, i = j);
    f1(1 ? i : j, 1 ? i : j);

    b = i;
    b = i = j;
    b = i = j && i;
    b = 1 ? i : j;
    b = 1, i;
    return i;
}
