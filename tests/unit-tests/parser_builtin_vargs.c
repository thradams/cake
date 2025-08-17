
typedef __builtin_va_list va_list;

int add_nums_C23(int count, ...)
{
    int result = 0;
    va_list args;
    __builtin_c23_va_start(args);
 
    
    for (int i = 0; i < count; ++i) {
        result += __builtin_va_arg(args, int);
    }
 
    __builtin_va_end(args);
    return result;
}

int main(void)
{    
    add_nums_C23(4 /*count*/, 25, 25, 50, 50);
}