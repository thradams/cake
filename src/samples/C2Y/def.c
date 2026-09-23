/*
  https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt
*/

#def foo(x)
		do {
			bar(x);
			baz(x);
		}
        while (0)
#enddef

foo(1)
foo(2)
