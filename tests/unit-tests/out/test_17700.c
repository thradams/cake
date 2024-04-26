#pragma safety enable


void free(void * /*_Owner*/ /*_Opt*/ p);
struct X {
    char * /*_Owner*/ text;
};

void x_destroy(/*_Opt*/ struct X * /*_Obj_owner*/ x) {
    free(x->text);
}

int main() {
    /*_Opt*/ struct X x = {0};
    x_destroy(&x);
}
