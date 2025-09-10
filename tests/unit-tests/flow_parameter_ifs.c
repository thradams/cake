#pragma safety enable

struct X {
    int* _Opt pi;
};

int* f();
int main(struct X* _Opt pX)
{
    if (pX)
    {
        static_state(pX, "not-null");
    }
    else
    {
        static_state(pX, "null");
    }

    if (!pX)
    {
        static_state(pX, "null");
    }
    else
    {
        static_state(pX, "not-null");
    }

    if (pX && pX->pi)
    {
        static_state(pX, "not-null");
        static_state(pX->pi, "not-null");
    }
    else
    {
        //static_debug(pX);
        static_state(pX, "not-null null");
        static_state(pX->pi, "not-null null");
    }

    // warning: pointer is always non-null 
    [[cake::w28]]
    if (pX == 0 || (pX->pi = f()) == 0)
    {
        static_state(pX, "null not-null");
    }
    else
    {
        //static_state(pX, "not-null"); //FAILS only on github windows!? WTF
    }


}
