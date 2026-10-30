#pragma safety enable

char* f();

void g()
{
    const char* s1 = f();    
    if (s1 == 0)
    {
        compile_assert(s1 == 0);
    }
    else
    {        
        compile_assert(s1 != 0);
    }
}
