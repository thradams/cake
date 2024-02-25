void* _Owner malloc(int sz);

void f(int i) {
    if (i) {
    }
    else {
        int* _Owner p3 = malloc(1);
    }
}
void dummy() {}
#pragma cake diagnostic check "-Wmissing-destructor"