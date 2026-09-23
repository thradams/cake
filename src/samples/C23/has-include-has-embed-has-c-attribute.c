
#if __has_include(<stdio.h>)
#warning  yes we have <stdio.h>
#endif


#if __has_embed(<stdio.h>)
#warning  yes we have <stdio.h> embed
#endif


#if __has_include(<otherfile.h>)
#warning  YES
#else
#warning  NO we dont have <otherfile.h>
#endif


#if __has_c_attribute(fallthrough)
#else
#warning at this moment we return 0 for all attributes
#endif

/*
  __has_include is a sample of feature that is impossible to translate, 
  unless for immediate compilation.
*/
