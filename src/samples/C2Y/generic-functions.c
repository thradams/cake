
#define SWAP(a, b)  (static void (typeof(a) arg1, typeof(b) arg2)) {     typeof(*a) temp = *arg1;    *arg1 = *arg2;    *arg2 = temp;   }(a, b)

int main()
{
    int a = 1;
    int b = 2;
    SWAP(&a, &b);
    SWAP(&a, &b);
}
