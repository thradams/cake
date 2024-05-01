#pragma safety enable

int * f();
int main()
{
    int *p;
    if ((p = f()) == 0)
    {
      static_state(p, "null");
    }
    else
    {
        static_state(p, "not-null");
    }    
}