// Test 6: Over-aligned type with unsupported alignment


alignas(4096) int over_aligned_var; // Error: Unsupported alignment for object of this storage duration

int main() {
    return 0;
}
