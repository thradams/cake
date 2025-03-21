#pragma safety enable


struct X {
    char* _Owner text;
};

void x_destroy(_Dtor struct X*  p);


int main() {
    struct X x;
    x_destroy(&x);
}

//flow analyze
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
