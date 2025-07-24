static int g;

int main()
{
    void f();

    static int s;
    int i;
    void local()
    {
        f(); //ok
        g;//ok
        s; //ok
        i; //error
    }
#pragma cake diagnostic check "-E1870"       

    (void())
    {
        f(); //ok
        g;//ok
        s; //ok
        i; //error
    };

#pragma cake diagnostic check "-E1870"       
}
