#pragma safety enable


void free(void * _Owner _Opt p);
struct X {
    char * _Owner text;
};

void x_destroy(_Opt _Dtor struct X *  x) {
    free(x->text);
}

int main() {
    _Opt struct X x = {};
    x_destroy(&x);
}
