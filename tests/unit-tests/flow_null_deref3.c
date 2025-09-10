#pragma safety enable


int main()
{
    int* _Opt ptr = 0;

    //warning: dereference a NULL object
    [[cake::w33]]
    int k = *ptr;
    k = 0;
    return 0;
}

