#pragma safety enable

enum E { Z = 0 };

int main()
{
    //warning: unusual expression/type used as null pointer constant

    int* _Opt p = false; //lint 46

    //warning: unusual expression/type used as null pointer constant

    p = '\0'; //lint 46

    //warning: unusual expression/type used as null pointer constant

    p = Z; //lint 46

    p = 0;

    //error: non-pointer to pointer 

    p = 1; //lint 1340

    //error: non-pointer to pointer 

    p = true; //lint 1340

}

