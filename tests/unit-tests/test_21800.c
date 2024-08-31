#pragma safety enable

void f_const(const int a);
void f(int a);


int main(void)
{
    int a;

    f_const(a);
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"

    f(a);
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
#pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
}