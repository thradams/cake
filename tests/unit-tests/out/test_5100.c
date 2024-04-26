
#pragma safety enable


struct Y { 
    int i;
};

struct X { 
    char * /*_Owner*/ /*_Opt*/ name;
};

int main()
{
    struct Y y1 = {0};
    struct Y y2 = {0};
    y1 = y2; //ok

    struct X x1 = {0};
    struct X x2 = {0};
    x1 = x2; //ok

}