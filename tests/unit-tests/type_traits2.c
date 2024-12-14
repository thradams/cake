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

const int i=0;
static_assert(_Generic(&i, const int *: 1));

const int *const p=0;
static_assert(_Generic(&p, const int *const *: 1));

int main()
{
	static_assert(!_is_lvalue(1));
	static_assert(_is_lvalue("a"));
	static_assert(_is_lvalue((int){0}));

	int a;
	static_assert(_is_lvalue(a));
	static_assert(_is_lvalue((a)));

	enum E
	{
		A
	};
	static_assert(!_is_lvalue(A));

	struct X x = {0};
	static_assert(_is_lvalue(x.i));

	struct X *px = 0;
	static_assert(_is_lvalue(px->i));

	//== Non-lvalue object expressions ==
	//
	// integer, character, and floating constants
	static_assert(!_is_lvalue(1));
	static_assert(!_is_lvalue(1.2));
	static_assert(!_is_lvalue('a'));

	// zero | not-zero function call expression
	static_assert(!_is_lvalue(f()));

	// zero | not-zero cast expression
	static_assert(!_is_lvalue((int)(0)));

	// member access operator (dot) applied to a non-lvalue structure/union
	static_assert(!_is_lvalue(f().i));

	// results of all arithmetic, relational, logical, and bitwise operators
	static_assert(!_is_lvalue(1 + 2));
	static_assert(!_is_lvalue(1 && 2));
	static_assert(!_is_lvalue(~1));

	// results of increment and decrement operators
	static_assert(!_is_lvalue(a++));

	// results of assignment operators

	// the conditional operator
	static_assert(!_is_lvalue(a ? a : a));

	// the comma operator
	static_assert(!_is_lvalue(a, a));

	// the address-of operator, even if neutralized by application to the result of unary * operator
	static_assert(!_is_lvalue(&a));
}
