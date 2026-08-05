#pragma safety enable

void f(int i, int c)
{
    if (c < 4)
    {
        if (c < 1)
        {
           return;
        }
        else
        {
            return;
        }
    }

    i = 0; //saying i end of lifetime
}
