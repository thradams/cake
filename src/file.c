extern int printf(char* format, ...);



int main()
{
    (void(typeof(1) val)) { static typeof(1) lastval; printf("%x\n", lastval); lastval = val;} (1);
    (void(typeof(1) val)) { static typeof(1) lastval; printf("two : %x\n", lastval); lastval = val;} (1);
    return 0;
}