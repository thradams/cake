// Test 10: has_c_attribute expression with non-identifier token

#if __has_c_attribute(123) // Error: Non-identifier token used in has_c_attribute
    #define ATTRIBUTE_PRESENT 1
#endif

int main() {
    return 0;
}
