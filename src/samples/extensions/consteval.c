
//syntax : const(constant-expression)
// This would be more useful when cake evaluate functions
// at compile time

int main(void)
{
    int i = 1;
    int j;
    j = const(5 + 3);

    // error
    // j = const(i + 3);
}
