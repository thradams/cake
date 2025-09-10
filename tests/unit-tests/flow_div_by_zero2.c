
#pragma safety enable

void  f(int i)
{
    int k = 1;
    int* p = &k;

    if (i)
    {
        *p = 0;
    }
    //possible division by zero
    [[cake::w36]]
    int h = 212 / k;
}

int main() {}

