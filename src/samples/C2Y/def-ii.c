  // Add directives #def and #enddef
  // https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3524.txt

/*
  use -preprocess-def-macro
  to preprocess #def macro after expansion
*/


#def foo(x)
		do {
#if 0
			bar(x);
#endif
			baz(x);
		}
        while (0)
#enddef

foo(2)
