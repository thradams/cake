#pragma safety enable
enum E1 { a };
enum E2 { b };
int main()
{
    //warning: operator '|' between enumerations of different types.
    [[cake::w4]]
    int i = a | b;

    //TODO add more..
}
