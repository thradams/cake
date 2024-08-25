
#pragma safety enable

void  f(int i)
{
    int k = 1;
    int * p = &k;
       
    if (i) {
       *p =0;
    }
    int h = 212/k;
    #pragma cake diagnostic check "-Wflow-div-by-zero"
}

int main() {}
