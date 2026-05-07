#pragma safety enable
enum E1 { a };
enum E2 { b };
int main()
{
    //

    int i = a | b; //lint 4 warning: operator '|' between enumerations of different types.

    //TODO add more..
}
