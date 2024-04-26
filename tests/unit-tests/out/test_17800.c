#pragma nullable enable
#pragma ownership enable


struct X {
    char * /*_Owner*/ /*_Opt*/ text;
};

void f(int condition) {
    struct X x1 = {0};
    struct X x2 = {0};
    struct X * p = condition ? &x1 : &x2;

    
}
