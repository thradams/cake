void f(int i) {}

int main()
{
    f((void*)0); //lint 75 pointer to integer conversion
}
