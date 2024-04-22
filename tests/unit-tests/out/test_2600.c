
#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)

struct X
{
    int i;
};

struct Y
{
    double d;
};

struct X *F() { return 0; }

int main()
{
    struct X x;
    struct Y y;

    
    

    

    int aa[10];

    
    

    int *p = 0;
    

    

    

    
    

    struct X x2;
    

    
    
    

    // static_assert(_is_same(typeof(A), int));

    
    
    

    
    
}
