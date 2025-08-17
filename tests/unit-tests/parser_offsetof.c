 
struct S {
    char c;
    double d;
};
 
int main(void)
{
    static_assert(__builtin_offsetof(struct S, c) == 0);
    static_assert(__builtin_offsetof(struct S, d) == 8);    
}