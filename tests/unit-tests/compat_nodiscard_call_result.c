/* the result of a [[nodiscard]] call is a value: A15.4 is only between function types */
#pragma safety enable

[[nodiscard]] _Clear void* _Owner my_calloc(unsigned long n, unsigned long s);
[[nodiscard]] void* _Owner my_alloc(unsigned long s);
void my_free(void* _Owner _Opt p);

struct Y { int value; };

int main()
{
    struct Y * _Owner y = my_calloc(1, sizeof(*y));
    struct Y * _Owner z = my_alloc(sizeof(*z));
    my_free(y);
    my_free(z);
}
