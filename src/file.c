int  main()
{
    char s[2];
    _Generic(s,
        char* : "char*",
        char[2] : "char [2]",
            default: "?");
}
