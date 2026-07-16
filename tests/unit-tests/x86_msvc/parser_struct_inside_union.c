/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct __tag1 {
    int * pSelect;
};

union __tag0 {
    struct __tag1 view;
};

struct X {
    union __tag0 u;
};

int main()
{
    struct X t;

    t.u.view.pSelect = 0;
}
