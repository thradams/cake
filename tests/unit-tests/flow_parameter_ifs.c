#pragma safety enable

struct X {
    int* _Opt pi;
};

int* f();
int main(struct X* _Opt pX)
{
    if (pX)
    {
        assert_state(pX, "not-null");
    }
    else
    {
        assert_state(pX, "null");
    }

    if (!pX)
    {
        assert_state(pX, "null");
    }
    else
    {
        assert_state(pX, "not-null");
    }

    if (pX && pX->pi)
    {
        assert_state(pX, "not-null");
        assert_state(pX->pi, "not-null");
    }
    else
    {
        //static_debug(pX);
        assert_state(pX, "not-null null");
        assert_state(pX->pi, "not-null null");
    }

    // warning: pointer is always non-null 
    [[cake::w28]]
    if (pX == 0 || (pX->pi = f()) == 0)
    {
        assert_state(pX, "null not-null");
    }
    else
    {
        //assert_state(pX, "not-null"); //FAILS only on github windows!? WTF
    }


}
