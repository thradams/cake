#pragma safety enable

/*
  returns a non-null pointer
*/
int * f();

int main()
{
    int * p;
    if ((p = f()) == 0)
//      ~~~~~~~~ 
    {
//      warning: pointer is always not-null [-Wflow-not-null]
#pragma cake diagnostic check "-Wflow-not-null"

        //if will not change the state
        static_state(p, "not-null");
    }
    else
    {
        static_state(p, "not-null");
    }
}