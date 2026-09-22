// Test 7: Alignment specifier specifying less strict alignment

alignas(4) double less_aligned_double; //lint 2130 requested alignment 4 is less than minimum alignment 8

int main() {
    return 0;
}
