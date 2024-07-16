#pragma safety enable


struct X {
    char* /*_Owner*/ text;
};

void x_destroy(struct X* /*_Obj_owner*/ p);


int main() {
    struct X x;
    x_destroy(&x);
}

//flow analyze
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
