int f(int condition)
{
    int* _Owner p = 0;
    try
    {
        int* _Owner p2 = p;
        if (condition) throw;
        p = 0;
    }
    static_debug(p); //shows "null" - it should be "null or moved"
}