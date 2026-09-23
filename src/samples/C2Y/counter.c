//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3457.htm#number-of-expansions

#define X(Z) Z Z
X(__COUNTER__) // 0 0
X(__COUNTER__) // 1 1
