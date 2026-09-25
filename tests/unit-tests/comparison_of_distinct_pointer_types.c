int f(int* a, double* b)
{
    return a == b; //lint 950 comparison of distinct pointer types ('int *' and 'double *')
}
