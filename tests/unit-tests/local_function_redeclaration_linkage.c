/* N3884 EXAMPLE 8: a block scope redeclaration cannot change the linkage */

void func(void) { }

void demo1(void)
{
    extern void func(void);
    static void func(void) { } //lint 1020 static declaration follows non-static declaration
    func();
}

void demo2(void)
{
    static void func(void);
    static void func(void) { }
    func();
}

void demo3(void)
{
    static void g(void);
    extern void g(void); //lint 1020 non-static declaration follows local function declaration
    static void g(void) { }
    g();
}
