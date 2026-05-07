#pragma safety enable


int main()
{
    int* _Opt ptr = 0;

    int k = *ptr; //lint 33 warning: dereference a NULL object
    k = 0;
    return 0;
}

