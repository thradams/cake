#pragma safety enable
enum E1 { a };
enum E2 { b };
int main()
{
    int i = a | b;
    //warning: operator '|' between enumerations of different types. (enum E1, enum E2) [-Wenum-conversion]
#pragma cake diagnostic check "-Wenum-conversion"

    //TODO add more..
}
