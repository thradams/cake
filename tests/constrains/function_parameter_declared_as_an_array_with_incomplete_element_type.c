// Test 5: Function parameter declared as an array with incomplete element type

struct Incomplete;

void func_with_incomplete_array_param(struct Incomplete arr[]); // Error: Array parameter cannot have incomplete element type

int main() {
    return 0;
}
