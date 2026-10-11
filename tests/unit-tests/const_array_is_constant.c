// elements of a const array initialized with constants are constants, like constexpr

const int a[] = { 1, 2, 3 };
static_assert(a[2] == 3);
static_assert(sizeof a == 3 * sizeof(int));

const char s[] = "abc";
static_assert(s[0] == 'a');

const int m[2][2] = { { 1, 2 }, { 3, 4 } };
static_assert(m[1][0] == 3);

const int b = a[1] + 1;
static_assert(b == 3);

int not_const[] = { 1, 2 };
static_assert(not_const[0] == 1); //lint 960

volatile const int v[] = { 1 };
static_assert(v[0] == 1); //lint 960

int main() {
    return 0;
}
