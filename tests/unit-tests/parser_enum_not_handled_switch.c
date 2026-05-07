#pragma safety enable

enum E { a, b, c };
void func(enum E e)
{
    // warning: enumeration 'c' not handled in switch
    
    switch (e)
    {
        case a:break;
        case b:break;
            //case c:;
    } //lint 45
}
int main() {}


