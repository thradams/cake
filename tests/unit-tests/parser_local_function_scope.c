static int g;

int main()
{
    void f();

    static int s;
    int i;
    void local()
    {
        f(); //ok
        g++;//ok
        s++; //ok
        //error: 'i' cannot be evaluated in this scope
        [[cake::e1870]]
        i++; //error
    }


    (void())
    {
        f(); //ok
        g++;//ok
        s++; //ok

        //error: 'i' cannot be evaluated in this scope
        [[cake::e1870]]
        i++; //error
    };

}
