// Test 5: Use of defined operator in limit constant expression

#define MAX_SIZE 10
#embed "resource.txt" limit(defined(MAX_SIZE)) // Error: defined operator not allowed in limit constant expression

int main() {
    return 0;
}
