#pragma safety enable


int strlen(const char* s);

int main()
{
    //warning: cannot convert a null pointer constant to non-nullable pointer
    [[cake::w60]]
    strlen(0);

    char* _Opt p = 0;

    //flow
    //warning: passing a possible null pointer 'p' to non-nullable pointer parameter
    [[cake::w35]]
    strlen(p);

}

