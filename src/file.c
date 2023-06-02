
[[cake::free]] void* malloc(int i) {}
void free([[cake::free]] void* p) {}

struct X {
    int i;
};

[[cake::free]] struct X* f() {
    struct X* p = malloc(1);
    struct X* p2;
    p2 = [[cake::move]] p;
    return p2; /*p2 is moved*/
}

int main() {
    struct X* p = f();

    /*we need call free here*/
    free(p);
}
