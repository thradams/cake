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
        compile_assert(i != 0);
    }
   
}
