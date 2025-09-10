#pragma safety enable


int* f();
int main()
{
    int* _Opt p = f();

    //pointer is always not-null
    [[cake::w28]]
    if (p)
        return 0;

}
