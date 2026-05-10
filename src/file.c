#pragma safety enable

struct X 
{
    int* _Opt a;
    int* _Opt b;
};

void f(struct X* p);
void f2(int** p);

int main()
{
    struct X x = {0};
    f2(&x.a);
    assert_state(x.a, "not-null null");    
}