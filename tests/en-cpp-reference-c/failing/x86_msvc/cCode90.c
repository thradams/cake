/* Cake x86_msvc */
struct error_info {
    int status;
};


struct error_info enable_missile_safety_mode()
{
    struct error_info  __v0;

    __v0.status = 0;
    return __v0;
}


void launch_missiles()
{
}


void test_missiles()
{
    enable_missile_safety_mode();
    launch_missiles();
}


static struct error_info  __c0_e;

struct error_info *foo()
{
    return &__c0_e;
}


void f1()
{
    foo();
}


int strategic_value(int x, int y)
{
    return x ^ y;
}


int main()
{
    int z;

    strategic_value(4, 2);
    z = strategic_value(0, 0);
    return z;
}


