// Test 1: Function declarator with function type as return type

int invalid_func(void)(void); // Error: Function cannot have a function type as return type

int main() {
    return 0;
}
