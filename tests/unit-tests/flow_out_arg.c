#pragma safety enable

bool init(_Ctor int *a) {
    *a = 3;
    return true;
}

int main() {
    int a;
    if (init(&a) || a == 0) return a;
    if (init(&a) && a == 0) return a;
    return a;
}