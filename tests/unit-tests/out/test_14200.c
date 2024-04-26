#pragma safety enable


int main() {
    int* /*_Opt*/ ptr = 0;
    int k = *ptr;
    k = 0;
    return 0;
}

#pragma cake diagnostic check "-Wanalyzer-null-dereference"
