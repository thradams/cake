#pragma safety enable

enum E { a, b, c };
void func(enum E e)
{
    // warning: enumeration 'c' not handled in switch
    [[cake::w45]]
    switch (e)
    {
        case a:break;
        case b:break;
            //case c:;
    }
}
int main() {}


