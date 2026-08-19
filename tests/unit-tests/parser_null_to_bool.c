void f(bool b);

int main(void)
{

    bool b = nullptr; //lint 49 75 warning: implicit conversion of nullptr constant to 'bool'


    b = nullptr; //lint 49 75 warning: implicit conversion of nullptr constant to 'bool'


    f(nullptr); //lint 49 75 warning: implicit conversion of nullptr constant to 'bool'

}
