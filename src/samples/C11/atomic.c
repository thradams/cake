/* each _Atomic operation becomes a call to a helper */
_Atomic int counter;
_Atomic(int*) p;
int arr[4];

int main(void)
{
    counter = 1;
    counter++;
    counter += 2;
    counter *= 3;
    int value = counter;

    p = arr;
    p++;

    return value;
}
