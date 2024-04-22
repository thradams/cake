void f1();
void f2(void);
void f3(char *s, ...);
int main()
{
    f1();
    f2();
    f3("");
    f3("", 1, 2, 3);
}
