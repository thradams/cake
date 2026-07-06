//..\tests\lounge\*.c -test-mode

int main()
{   
    constexpr bool b = 2;
    static_assert(b == true);    

    //constexpr unsigned char c = 300;
    //static_assert(b == true);    



    constexpr int i = 1;
    static_assert(i == 1);    

    constexpr double d = 1.2;    
    static_assert(d == 1.2);  
}
