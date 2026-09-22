// Test 1: Function declarator with function type as return type

int invalid_func(void)(void); //lint 1410 function returning function

int main() {
    return 0;
}
