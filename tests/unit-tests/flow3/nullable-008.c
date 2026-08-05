#pragma nullable enable

struct X { char* name; };

struct X return_struct()
{
    struct X* p;
    {
        struct X x;
        p = &x; //lint 30 passing a possible uninitialized object '(*&x).name' (see line 9)
    }
    // static_debug(*p);
    return *p; //lint 31 object '*p' lifetime has ended (see line 11)
}
