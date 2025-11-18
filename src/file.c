#pragma safety enable

struct X {
    char* _Opt name;
};


bool is_empty(struct X* p)
in{

}
out(bool r)
{
    assert(p->name != 0);
};


int main()
{
    struct X x = {};
    if (is_empty(&x))
    {
        static_debug(x);
    }
}
