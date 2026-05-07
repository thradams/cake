#pragma safety enable

/*
  returns a non-null pointer
*/
int* f();

int main()
{
    int* p;

    if ((p = f()) == 0) //lint 28 pointer is always non-null
    {
        //if will not change the state
        assert_state(p, "not-null");
    }
    else
    {
        assert_state(p, "not-null");
    }
}
