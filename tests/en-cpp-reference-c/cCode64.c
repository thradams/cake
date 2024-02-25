//en.cppreference.com/w/c/io/setbuf.html
#include <stdio.h>
#include <threads.h>
 
int main(void)
{
    setbuf(stdout, NULL); // unbuffered stdout
    putchar('a'); // 'a' appears immediately if stdout is unbuffered
    thrd_sleep(&(struct timespec){.tv_sec=1}, NULL); // sleep 1 sec
    putchar('b'); 
}