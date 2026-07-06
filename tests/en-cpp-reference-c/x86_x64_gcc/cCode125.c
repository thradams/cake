/* Cake x86_x64_gcc */
struct __tag3 {
    int arr[2];
};


extern int printf(char * __format, ...);

int main(void)
{
    int i;
    int j;
    int k;
    char c;
    char * p;
    char ** cpp;
    int arr1[2];
    int arr2[2];
    struct __tag3  sam1;
    struct __tag3  sam2;

    i = 1;
    j = 2;
    k = 3;
    i = j = k;
    printf("%d %d %d\n", i, j, k);
    c = 65;
    p = &c;
    cpp = &p;
    *cpp = &c;
    printf("%c \n", **cpp);
    cpp = 0;
    arr1[0] = 1;
    arr1[1] = 2;
    arr2[0] = 3;
    arr2[1] = 4;
    printf("arr1[0]=%d arr1[1]=%d arr2[0]=%d arr2[1]=%d\n", arr1[0], arr1[1], arr2[0], arr2[1]);
    sam1.arr[0] = 5;
    sam1.arr[1] = 6;
    sam2.arr[0] = 7;
    sam2.arr[1] = 8;
    sam1 = sam2;
    printf("%d %d \n", sam1.arr[0], sam1.arr[1]);
}


