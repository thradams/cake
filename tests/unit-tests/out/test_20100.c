#pragma safety enable

enum E { a, b, c };
void func ( enum E e )
{
    switch(e)
    {
        case a:break;
        case b:break;
        //case c:;
    }
    //not implemented yet
    //warning: enumeration value 'c' not handled in switch
    #pragma cake diagnostic check "-Wswitch" 
}
int main(){}


