/* C23 6.10.1: __has_c_attribute in #if, including when followed by an
   operator (the closing ')' used to be popped twice, eating the '==') */

#if !defined(__has_c_attribute)
#error "__has_c_attribute must be predefined"
#endif

#if __has_c_attribute(nodiscard) != 202311L
#error "nodiscard"
#endif

#if __has_c_attribute(nodiscard) == 0
#error "nodiscard == 0"
#endif

#if __has_c_attribute(fallthrough) != 202311L
#error "fallthrough"
#endif

#if __has_c_attribute(maybe_unused) != 202106L
#error "maybe_unused"
#endif

#if __has_c_attribute(deprecated) != 201904L
#error "deprecated"
#endif

#if __has_c_attribute(noreturn) != 202202L
#error "noreturn"
#endif

#if __has_c_attribute(no_such_attribute) != 0
#error "unknown attribute must be 0"
#endif

#if !__has_c_attribute(nodiscard) || __has_c_attribute(no_such_attribute)
#error "logical operators"
#endif

#if (__has_c_attribute(nodiscard)) == 0
#error "parenthesized"
#endif

int main(void)
{
    return 0;
}
