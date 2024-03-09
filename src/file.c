void* f();

void f1()
{
    int i = 0;
    void* p = f();
    static_debug(p);
    try
    {
        if (f())
        {
         //   static_debug(p);
        }
        else
        {
           // static_debug(p);
            if (f())
            {
             //   static_debug(p);
            }
            else
            {
                static_debug(p);
                throw;
            }
        }
    }
    catch
    {
    }
    static_debug(p);
}
