#pragma nullable enable
#pragma ownership enable

void free(void * _Owner _Opt p);
struct X {
    char * _Owner text;
};

void x_destroy(_Opt struct X * _Obj_owner x) {
    free(x->text);
}

int main() {
    struct X x = {};
    x_destroy(&x);
}
