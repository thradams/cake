#include <stdio.h>

void f() {
    printf("extern function\n");
}

int main()
{    
    static void f(); //forward declaration

    static void f() {
        printf("local function\n");
    }

    f(); //calls local function
}
