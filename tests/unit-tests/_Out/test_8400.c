#pragma safety enable


void* /*_Owner*/ /*_Opt*/ malloc(int sz);

void f(int i) {
    if (i) {
    }
    else {
        int* /*_Owner*/ /*_Opt*/ p3 = malloc(1);
    }
}
#pragma cake diagnostic check "-Wmissing-destructor"

