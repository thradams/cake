#pragma safety enable

enum E {Z = 0};

int main()
{
    int* _Opt p = false;
#pragma cake diagnostic check "-Wunusual-null"

    p = '\0';
#pragma cake diagnostic check "-Wunusual-null"

    p = Z;
#pragma cake diagnostic check "-Wunusual-null"

    p = 0;



    p = 1;
    //error: non-pointer to pointer [E133]
#pragma cake diagnostic check "-E133"

    p = true;
    //error: non-pointer to pointer [E133]
#pragma cake diagnostic check "-E133"
}
