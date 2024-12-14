#pragma safety enable

void f(int* p)
{
    if (p != 0) {}
#pragma cake diagnostic check "-Wflow-not-null"

    if (0 != p) {}
#pragma cake diagnostic check "-Wflow-not-null"

    if (p == 0) {}
 #pragma cake diagnostic check "-Wflow-not-null"

    if (0 == p) {}
#pragma cake diagnostic check "-Wflow-not-null"

}