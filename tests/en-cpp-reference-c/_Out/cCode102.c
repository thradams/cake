//en.cppreference.com/w/c/language/continue.html
#include <stdio.h>
 
int main(void) 
{
    for (int i = 0; i < 10; i++) {
        if (i != 5) continue;
        printf("%d ", i);             // this statement is skipped each time i != 5
    }
 
    printf("\n");
 
    for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 5; k++) { // only this loop is affected by continue
            if (k == 3) continue;
            printf("%d%d ", j, k);    // this statement is skipped each time k == 3
        }
    }
}