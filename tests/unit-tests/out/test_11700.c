#pragma safety enable


struct Y {
    char* /*_Owner*/ p0;
    int* /*_Owner*/ p2;
    double i2;
};

struct X {
    char* /*_Owner*/ text;
    int* /*_Owner*/ p1;
    int i;
    struct Y* /*_Opt*/ pY;
};

struct X f();
void destroy(struct X * /*_Obj_owner*/ x);

int main()
{
    struct X x;
    x = f();
    
    
    
    
        
    destroy(&x);
}
