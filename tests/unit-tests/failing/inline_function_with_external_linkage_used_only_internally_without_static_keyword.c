// Test 8: Inline function with external linkage used only internally without static keyword


inline void inline_func() { // Error: Inline function used only internally should be marked static
    //printf("Hello\n");
}
