
double sample_stddev(int count, ...)
{
    /* Compute the mean with args1. */
    double sum = 0;
    __builtin_va_list args1;
    __builtin_va_start(args1, count);
    __builtin_va_list args2;
    __builtin_va_copy(args2, args1);   /* copy va_list object */
    for (int i = 0; i < count; ++i)
    {
        double num = __builtin_va_arg(args1, double);
        sum += num;
    }
    __builtin_va_end(args1);
    double mean = sum / count;

    /* Compute standard deviation with args2 and mean. */
    double sum_sq_diff = 0;
    for (int i = 0; i < count; ++i)
    {
        double num = __builtin_va_arg(args2, double);
        sum_sq_diff += (num - mean) * (num - mean);
    }
    __builtin_va_end(args2);
    return 0;
}