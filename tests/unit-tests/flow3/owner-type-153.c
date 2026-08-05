#pragma safety enable
int f(int condition)
{
    int * _Owner _Opt p = 0;
    try
    {
        int * _Owner _Opt p2 = p;

        //p is still null here because null pointers are not moved.
        compile_assert(p == 0); //lint 67 compile_assert failed: value could be any value, including zero, set at line 10 in "try

        if (condition) throw;
        p = 0;
    }
    catch
    {
    }
    
    compile_assert(p == 0); //lint 67 compile_assert failed: value could be any value, including zero, set at line 19 in "root
}
