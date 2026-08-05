#pragma safety enable


int strlen(const char* s);

int main()
{
    //warning: cannot convert a null pointer constant to non-nullable pointer

    strlen(0); //lint 35 cannot convert a null pointer constant to non-nullable pointer; passing a possible null

    char* _Opt p = 0;

    //flow
    //warning: passing a possible null pointer 'p' to non-nullable pointer parameter

    strlen(p); //lint 35 passing a possible null pointer 'p' to non-nullable pointer parameter (see line 12)

}
