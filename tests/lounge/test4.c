struct X 
{
   int a;
   int b;
   int c;
};

struct Y
{
   struct X x;   
   struct X x2;   
   int d;
};

int main()
{   
    constexpr struct Y y = { .x = {.a =1, 2}, 3};  
    static_assert(y.x.a == 1);
    static_assert(y.x.b == 2);
    static_assert(y.x2.a == 3);    
}
