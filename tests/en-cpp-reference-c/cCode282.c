//en.cppreference.com/w/c/numeric/random/rand.html
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
int main(void)
{
    srand(time(NULL)); // use current time as seed for random generator
    int random_variable = rand();
    printf("Random value on [0,%d]: %d\n", RAND_MAX, random_variable);
 
    // roll a 6-sided die 20 times
    for (int n=0; n != 20; ++n) {
        int x = 7;
        while(x > 6) 
            x = 1 + rand()/((RAND_MAX + 1u)/6); // Note: 1+rand()%6 is biased
        printf("%d ",  x); 
    }
}