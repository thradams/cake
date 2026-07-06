/* Cake x86_x64_gcc */

extern double sqrt(double __x);

double sample_stddev(int count, ...)
{
    double sum;
    __builtin_va_list args1;
    __builtin_va_list args2;
    double mean;
    double sum_sq_diff;

    sum = 0;
    __builtin_va_start(args1, count);
    __builtin_va_copy(args2, args1);
    {
        int i;

        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = __builtin_va_arg(args1, double);
            sum += num;
        }
    }
    __builtin_va_end(args1);
    mean = sum / count;
    sum_sq_diff = 0;
    {
        int i;

        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = __builtin_va_arg(args2, double);
            sum_sq_diff += (num - mean) * (num - mean);
        }
    }
    __builtin_va_end(args2);
    return sqrt(sum_sq_diff / count);
}


extern int printf(char * __format, ...);

int main(void)
{
    printf("%f\n", sample_stddev(4, 25.0, 27.300000000000001, 26.899999999999999, 25.699999999999999));
}


