// Test 2: Struct member with incomplete type

struct Incomplete;

struct TestStruct {
    struct Incomplete member; //lint 740 member has incomplete type
};

int main() {
    return 0;
}
