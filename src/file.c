#include <ctype.h>
 
int main(void)
{
    unsigned char c = '\xdf'; // German letter ß in ISO-8859-1
 
    isalnum(c);
     
}