#pragma safety enable



int f();
int main()
{
    int i;
    try
    {
        if (f()){
            i = 1;
            throw;
        }
        i = 0;
    }
    catch
    {
        static_state(i, "not-zero");
    }
    static_state(i, "zero | not-zero");
}