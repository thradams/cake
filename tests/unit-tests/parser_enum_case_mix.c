#pragma safety enable
enum E1 { A, B };
enum E2 { C, D };

void f(enum E1 e)
{
    switch (e)
    {
        case A:
            break;

        //warning: mismatch in enumeration types (enum E2, enum E1)
        [[cake::w4]]
        case D:
            break;
    }
}
int main() {}
