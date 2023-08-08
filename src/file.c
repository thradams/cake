/* Compilers that lack __has_attribute may object to
       #if defined __has_attribute && __has_attribute (...)
   even though they do not need to evaluate the right-hand side of the &&.
   Similarly for __has_builtin, etc.  */
#if (defined __has_attribute \
     && (!defined __clang_minor__ \
         || 3 < __clang_major__ + (5 <= __clang_minor__)))
# define __glibc_has_attribute(attr) __has_attribute (attr)
#else
# define __glibc_has_attribute(attr) 0
#endif
#ifdef __has_builtin
# define __glibc_has_builtin(name) __has_builtin (name)
#else
# define __glibc_has_builtin(name) 0
#endif
#ifdef __has_extension
# define __glibc_has_extension(ext) __has_extension (ext)
#else
# define __glibc_has_extension(ext) 0
#endif
