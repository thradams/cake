extern int printf(char * format, ...);
#def Funny(V)
    (void (typeof(V) val))
    {
        static typeof(V) lastval;
        printf("%x\n",lastval);
        lastval = val;
    }
    (V)
#enddef

int main()
{
    Funny(1);
    Funny(2);
    Funny(3);
    return 0;
}
