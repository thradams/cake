
int f(int a[2])
{
    int i;
    
    
    i = sizeof(a); //lint 47 warning: sizeof applied to array function parameter 

    //warning: sizeof applied to array function parameter

    static_assert(sizeof(a) == sizeof(void*));//lint 47
}

int main()
{
    int i;
    //error: struct is incomplete type
    
    i = sizeof(struct X); //lint 740
}

