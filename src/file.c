

struct X {
    int i;
};

struct Y {
    double d;
};

enum E { A = 1 };
enum E e1;
struct X* F() { return 0; }

int main()
{
    enum E { B } E2; 
    //static_assert(typeid(e2) == typeid(enum E));
    //static_assert(typeid(e2) != typeid(e1));

    struct X x;
    struct Y y;

    static_assert(typeid(x) == typeid(struct X));
    static_assert(typeid(x) != typeid(struct Y));

    static_assert(typeid(int(double)) != typeid(int()));
    int aa[10];

    static_assert(typeid(*F()) == typeid(struct X));
    static_assert(typeid(&aa) == typeid(int(*)[10]));

    int* p = 0;
    static_assert(typeid(*(p + 1)) == typeid(int));
    
    static_assert(1 == typeid(int));

    static_assert(typeid(main) == typeid(int()));
    
    
    static_assert(typeid(main) != typeid(int(double)));
    static_assert(typeid(main) != typeid(int));


    struct X x;
    enum E e;
    static_assert(typeid(e) == typeid(enum E));
    static_assert(typeid(x) == typeid(struct X));
    static_assert(typeid(e) != typeid(struct X));

    
        
    static_assert(1L == typeid(long));
    static_assert(1UL == typeid(unsigned long));
    static_assert(1ULL == typeid(unsigned long long));
    static_assert(A == typeid(int));
    static_assert(1.0 == typeid(double));
    static_assert(1.0f == typeid(float));
    static_assert(1.0L == typeid(long double));
  
}
