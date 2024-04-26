// https://en.cppreference.com/w/c/language/value_category

struct X
{
	int i;
};

struct X f()
{
	struct X x = {0};
	return x;
}

const int i;


const int *const p;


int main()
{
	
	
	

	int a;
	
	

	enum E
	{
		A
	};
	

	struct X x = {0};
	

	struct X *px = 0;
	

	//== Non-lvalue object expressions ==
	//
	// integer, character, and floating constants
	
	
	

	// zero | not-zero function call expression
	

	// zero | not-zero cast expression
	

	// member access operator (dot) applied to a non-lvalue structure/union
	

	// results of all arithmetic, relational, logical, and bitwise operators
	
	
	

	// results of increment and decrement operators
	

	// results of assignment operators

	// the conditional operator
	

	// the comma operator
	

	// the address-of operator, even if neutralized by application to the result of unary * operator
	
}
