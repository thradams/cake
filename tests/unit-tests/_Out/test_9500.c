#pragma safety enable


struct X {
    char* /*_Owner*/ p;
};
void x_destroy(struct X* /*_Obj_owner*/ p);
void f(struct X* x)
{
    x_destroy(x);
}

#pragma cake diagnostic check "-Wmust-use-address-of"
