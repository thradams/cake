#pragma nullable enable

struct X { char* name; };

struct X * f()
{
    struct X* p;
    {
        struct X x;
        p = &x; //lint 30 passing a possible uninitialized object '(*&x).name' (see line 9)

    }
    
    return p; //lint 31 : passing a possible uninitialized object '(*&x).name' (see line 9)

}

