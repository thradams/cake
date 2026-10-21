#pragma safety enable

/* 85 is off by default and the suite passes -wd85; enable it, silence 44 */
#pragma cake diagnostic warning 85
#pragma cake diagnostic ignored 44

/* a constant condition is already handled by the parser (warning 44); flow must not repeat it as 85 "condition is always true" */

enum { K = 1 };

int main()
{
    int r = 0;
    if (K)
        r = 1;
    return r;
}
