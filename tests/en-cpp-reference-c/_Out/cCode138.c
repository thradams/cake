//en.cppreference.com/w/c/language/operator_other.html
#include <assert.h>
 
enum vehicle { bus, airplane, train, car, horse, feet };
 
enum vehicle choose(char arg)
{
    return arg == 'B' ? bus      :
           arg == 'A' ? airplane :
           arg == 'T' ? train    :
           arg == 'C' ? car      :
           arg == 'H' ? horse    :
                        feet     ;
}
 
int main(void)
{
    assert(choose('H') == horse && choose('F') == feet);
}