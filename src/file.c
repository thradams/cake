

/*
  Header ownership.h defines macros owner, view etc.
  It also "activate" ownership checks.
  If the compiler does not suports ownership it defines as empty macros
*/

#include <ownership.h> 
#include <stdlib.h>
#include <string.h>

struct X {
	char* owner name;
};

void x_destroy(struct X x)
{
	free(x.name);
}

int main() {
	struct X x = { 0 };
	x.name = strdup("a");
	x_destroy(x);
}

