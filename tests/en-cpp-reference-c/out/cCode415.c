//en.cppreference.com/w/c/thread/call_once.html
#include <stdio.h>
#include <threads.h>
 
void do_once(void) {
    puts("called once");
}
 
static once_flag flag = ONCE_FLAG_INIT;
int func(void* data)
{
    call_once(&flag, do_once);
}
 
int main(void)
{
    thrd_t t1, t2, t3, t4;
    thrd_create(&t1, func, NULL);
    thrd_create(&t2, func, NULL);
    thrd_create(&t3, func, NULL);
    thrd_create(&t4, func, NULL);
 
    thrd_join(t1, NULL);
    thrd_join(t2, NULL);
    thrd_join(t3, NULL);
    thrd_join(t4, NULL);
}