/* Cake 0.12.26 x86_msvc */
struct __tag1 {
    int * pSelect;
};

union __tag0 {
    struct __tag1  view;
};

struct X {
    union __tag0  u;
};


int main()
{
    struct X  t;

    t.u.view.pSelect = 0;
}


