int main(void)
{
    constexpr int arr[] = { 12 };
    static_assert(arr[0]==12);
    static_assert(_Countof(arr)==1);
    return 0;
}
