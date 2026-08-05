#pragma safety enable


void past_end(int i) {
    int v[3] = {1,2,3};
    if (i >= 3) v[i] = 0; //lint 70 warning: past the end of the array
}
void negative(int i) {
    int v[3] = {1,2,3};
    if (i < 0) v[i] = 0; //lint 70 warning: array index is negative
}
void branch(int c) {
    int v[3] = {1,2,3};
    int i; if (c) i = 0; else i = 5;
    v[i] = 0; //lint 70 warning: the i==5 path is out of bounds
}
void literal(void) {
    int v[3] = {1,2,3};
    v[5] = 0; //lint 42 warning (from the parser): index 5 past end
}

