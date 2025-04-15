
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
