// Test 5: has_c_attribute expression with invalid attribute name

#if __has_c_attribute(invalid-attribute-name) // Error: Invalid attribute name format for has_c_attribute
    #define ATTRIBUTE_SUPPORTED 1
#endif

int main() {
    return 0;
}
