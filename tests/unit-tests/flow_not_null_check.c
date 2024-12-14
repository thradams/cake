#pragma safety enable

char* _Opt f();

void f()
{
    const char* _Opt s1 = f();    
    if (s1 != 0)
    {
        static_state(s1, "not-null");        
    }
    else
    {        
        static_state(s1, "null");
    }
}