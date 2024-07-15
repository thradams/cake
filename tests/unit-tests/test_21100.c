#pragma safety enable
int f(int condition)
{
    int * _Owner _Opt p = 0;
    try
    {
        int * _Owner _Opt p2 = p;

        //p is still null here because null pointers are not moved.
        static_state(p, "null");

        if (condition) throw;
        p = 0;
    }
    catch
    {
    }
    
    static_state(p, "null");
}
