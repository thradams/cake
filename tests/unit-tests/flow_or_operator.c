#pragma safety enable

void f(int i)
{
    int j;
    if (i || j){
    }
    #pragma cake diagnostic check "-Wanalyzer-maybe-uninitialized"
}