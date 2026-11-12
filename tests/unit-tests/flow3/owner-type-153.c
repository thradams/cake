#pragma safety enable
void f(int condition)
{
    int * _Owner p = 0;
    try
    {
        int * _Owner p2 = p;

        //p is still null here because null pointers are not moved.
        compile_assert(p == 0);

        if (condition) throw;
        p = 0;
    }
    catch
    {
    }
    
    compile_assert(p == 0);
}
