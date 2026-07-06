/* Cake x86_x64_gcc */

extern double sqrt(double __x);

double stddev(int count, ...)
{
    double sum;
    double sum_sq;
    __builtin_va_list args;

    sum = 0;
    sum_sq = 0;
    __builtin_va_start(args, count);
    {
        int i;

        i = 0;
        for (; i < count; ++i)
        {
            double num;

            num = __builtin_va_arg(args, double);
            sum += num;
            sum_sq += num * num;
        }
    }
    __builtin_va_end(args);
    return sqrt(sum_sq / count - (sum / count) * (sum / count));
}


extern int printf(char * __format, ...);

int main(void)
{
    printf("%f\n", stddev(4, 25.0, 27.300000000000001, 26.899999999999999, 25.699999999999999));
}


