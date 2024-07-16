#pragma safety enable

char* /*_Opt*/ f();
int strcmp(const char * s1, const char *s2);
void f()
{
    const char* /*_Opt*/ s1 = f();    
    if (s1 && strcmp(s1, "a")==0)
    {
                
    }
    else
    {        
        
    }
}