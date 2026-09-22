// Test 7: Function parameter declared as a function type


void func_with_function_type_param(int param(void)); // Error: Parameter cannot have a function type

int main() {
    return 0;
}
