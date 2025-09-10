#pragma safety enable

/*
  returns a non-null pointer
*/
int* f();

int main()
{
    int* p;

    //pointer is always non-null
    [[cake::w28]]
    if ((p = f()) == 0)
    {
        //if will not change the state
        static_state(p, "not-null");
    }
    else
    {
        static_state(p, "not-null");
    }
}
