#pragma flow enable

/* False positive: the array is written by one loop and read by another over
   the same range, so every element read was written. The flow analysis does
   not relate the two loops (both run only when n > 0), and on the first
   iteration i == 0 it sees a[0] unwritten on the path where the first loop did
   not run. Only the condition form reports it: `s += a[i]` does not.
   Seen in cc89 abi_sysv.c (on_stack[i]). */

int count_set(int n)
{
    int a[4];
    for (int i = 0; i < n; i++)
        a[i] = 1;

    int s = 0;
    for (int i = 0; i < n; i++)
        if (a[i]) /* expected: no warning */
            s++;
    return s;
}
