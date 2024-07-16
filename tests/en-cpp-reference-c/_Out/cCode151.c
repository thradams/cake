//en.cppreference.com/w/c/language/thread_storage_duration.html
const double PI = 3.14159;         /* const variable is global to all threads  */
_Thread_local unsigned int seed;   /* seed is a thread-specific variable       */
 
int main(void)
{
    return 0;
}