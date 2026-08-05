#pragma safety enable

char* _Opt f();

void g()
{
    const char* _Opt s1 = f();    
    if (s1 != 0)
    {
        compile_assert(s1 != 0);
    }
    else
    {        
        compile_assert(s1 == 0);
    }
}
