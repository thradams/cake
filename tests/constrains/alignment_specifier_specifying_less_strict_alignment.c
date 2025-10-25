// Test 7: Alignment specifier specifying less strict alignment

alignas(4) double less_aligned_double; // Error: Alignment specifier cannot specify less strict alignment

int main() {
    return 0;
}
