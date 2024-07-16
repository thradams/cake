#pragma safety enable
enum E1 { A, B };
enum E2 { C, D };

void f(enum E1 e)
{
    switch (e)
    {
        case A:
            break;
        case D:
            break;
#pragma cake diagnostic check "-Wenum-conversion"
    }
}
int main() {}
