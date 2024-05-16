#pragma safety enable

char* _Opt f();

void f()
{
    const char* _Opt s1 = f();    
    if (!s1)
    {
        static_state(s1, "null");        
    }
    else
    {        
        static_state(s1, "not-null");
    }
}