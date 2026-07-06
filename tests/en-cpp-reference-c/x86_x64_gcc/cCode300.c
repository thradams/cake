/* Cake x86_x64_gcc */
struct __tag3 {
    unsigned int long __val[16];
};

struct __jmp_buf_tag {
    int long __jmpbuf[8];
    int __mask_was_saved;
    struct __tag3  __saved_mask;
};


struct __jmp_buf_tag my_jump_buffer[1];
extern int printf(char * __format, ...);
extern void longjmp(struct __jmp_buf_tag __env[1], int __val);

void foo(int status)
{
    printf("foo(%d) called\n", status);
    longjmp(my_jump_buffer, status + 1);
}


extern int _setjmp(struct __jmp_buf_tag __env[1]);

int main(void)
{
    volatile int count;

    count = 0;
    if (_setjmp(my_jump_buffer) != 5)
    {
        foo(++count);
    }
}


