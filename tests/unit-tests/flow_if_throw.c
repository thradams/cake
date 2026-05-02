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
        assert_state(i, "not-zero");
    }
    assert_state(i, "zero | not-zero");
}