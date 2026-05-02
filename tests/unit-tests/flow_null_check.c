#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (s1 == 0)
    {
        assert_state(s1, "null");        
    }
    else
    {        
        assert_state(s1, "not-null");
    }
}
