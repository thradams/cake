﻿#pragma safety enable

bool init(_Out int *a) {
    *a = 3;
    return true;
}

int main() {
    int a;
    if (0 || init(&a)) return a;
    return a;
}
