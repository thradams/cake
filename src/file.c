
#define SWAP(a, b)\
  (void (typeof(a)* arg1, typeof(b)* arg2)) { \
    typeof(a) temp = *arg1; *arg1 = *arg2; *arg2 = temp; \
  }(&(a), &(b))

#pragma expand SWAP
int main()
{
    int a;
    int b;
    SWAP(a, b);
}