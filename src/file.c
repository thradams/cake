void f(int condition)
{
    int i;
    
    if (condition)
    {    
       // i = 1;
    }
    else
    {
        i = 0;
    }
    static_debug_ex(i);
    //uninitialized not-zero OR uninitialized zero
}