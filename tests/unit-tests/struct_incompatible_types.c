struct X { int x; };
struct Y { int y; };
union U { int i; };
typedef struct { int a; } T;

void f(struct X x);
struct X g(void);

int main()
{
    struct X x;
    struct Y y;
    union U u;
    T t;

    f(x);
    f(g());
    struct X x2 = x;
    x2 = g();
    T t2 = t;

    f(y);   //lint 950
    f(u);   //lint 950
    f(t);   //lint 950
    f(1);   //lint 950
    x = y;  //lint 950
    int i = x; //lint 950
}
