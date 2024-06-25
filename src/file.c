#pragma safety enable
enum E1 {A, B};
enum E2 {C, D};

void f(enum E1 e)
{
    switch(e)
    {
        case 2:break;        
            case 2:break;        
    }
}
int main(){}
