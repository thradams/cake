#pragma safety enable

enum E { Z = 0 };

int main()
{
    //warning: unusual expression/type used as null pointer constant
    [[cake::w46]]
    int* _Opt p = false;

    //warning: unusual expression/type used as null pointer constant
    [[cake::w46]]
    p = '\0';

    //warning: unusual expression/type used as null pointer constant
    [[cake::w46]]
    p = Z;

    p = 0;

    //error: non-pointer to pointer 
    [[cake::e1340]]
    p = 1;

    //error: non-pointer to pointer 
    [[cake::e1340]]
    p = true;

}

