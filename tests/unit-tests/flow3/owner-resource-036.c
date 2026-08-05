#pragma safety enable


void free(void * _Owner _Opt p);
struct X {
    char * _Owner text;
};

void x_destroy(_Dtor struct X *  x) {
    free(x->text);
}

int main() {
    struct X x = {};
    x_destroy(&x); // clean: a _Dtor parameter accepts a partially-created
                   // object, so a null member is allowed (see dtor_is_opt.c).

}
