struct X {
    char* _Owner text;
};

void x_destroy(struct X* _Obj_owner p);


int main()
{
    struct X x;
    static_debug(x);
    x_destroy(&x);
}



void dummy()
{
}

//flow analyze
//#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
