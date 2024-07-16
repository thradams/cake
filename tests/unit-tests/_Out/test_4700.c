#pragma safety enable


char* /*_Opt*/ f() {
    char* /*_Owner*/ /*_Opt*/ p = 0;
    return p;

//cannot return a owner to non owner
#pragma cake diagnostic check "-E1280"

}
