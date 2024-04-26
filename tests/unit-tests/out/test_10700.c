#pragma safety enable


struct X { char* /*_Owner*/ text; };

void x_destroy(struct X* /*_Obj_owner*/ p);

void x_delete(struct X* /*_Owner*/ /*_Opt*/ p)
{
    if (p)
    {
        x_destroy(p);
        //memory pointed by p not deleted
    }
}
#pragma cake diagnostic check "-Wmissing-destructor"



