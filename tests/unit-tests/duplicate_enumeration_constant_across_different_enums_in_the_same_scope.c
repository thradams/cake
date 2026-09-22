// Duplicate enumeration constant declared in two different enums in the same scope

int main()
{
    enum X { A = 1 };
    enum Y { A = 2 }; //lint 2210

    return 0;
}
