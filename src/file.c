
enum E2 
{
    A
};

struct X2 {  int member2; };

int main(void)
{
    enum  E2 e = A;
    struct X2 {  int member2; } a;
    struct X2 {  int member2; } b;
    
    a.member2 = 2;
    return 0;
}
