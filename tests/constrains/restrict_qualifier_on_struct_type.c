// Test 6: restrict qualifier on struct type

struct Point {
    int x, y;
};

struct Point restrict p; // Error: restrict qualifier cannot be applied to a struct type

int main() {
    return 0;
}
