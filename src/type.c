/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/


#pragma safety enable

#include "ownership.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "error.h"

#include "parser.h"
#include "type.h"

size_t get_align_void_ptr(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 2;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_void_ptr(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 2;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;

}

size_t get_align_char(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 1;
    case TARGET_X86_MSVC:     return 1;
    case TARGET_X64_MSVC:     return 1;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 1;
    case TARGET_CATALINA:     return 1;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_char(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 1;
    case TARGET_X86_MSVC:     return 1;
    case TARGET_X64_MSVC:     return 1;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 1;
    case TARGET_CATALINA:     return 1;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;

}

size_t get_align_bool(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 1;
    case TARGET_X86_MSVC:     return 1;
    case TARGET_X64_MSVC:     return 1;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 1;
    case TARGET_CATALINA:     return 1;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_bool(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 1;
    case TARGET_X86_MSVC:     return 1;
    case TARGET_X64_MSVC:     return 1;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 1;
    case TARGET_CATALINA:     return 1;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_align_short(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 2;
    case TARGET_X86_MSVC:     return 2;
    case TARGET_X64_MSVC:     return 2;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 2;
    case TARGET_CATALINA:     return 2;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_short(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 2;
    case TARGET_X86_MSVC:     return 2;
    case TARGET_X64_MSVC:     return 2;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 2;
    case TARGET_CATALINA:     return 2;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_align_int(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 4;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 4;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 2;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_int(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 4;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 4;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 2;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_align_long(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 4;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 4;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_long(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 4;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 4;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_align_long_long(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 8;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 4;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_long_long(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 8;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 4;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");

    assert(false);
    return 0;
}


size_t get_align_float(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 4;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 4;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 4;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_float(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 4;
    case TARGET_X86_MSVC:     return 4;
    case TARGET_X64_MSVC:     return 4;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 4;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_align_double(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 8;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 8;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_double(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 8;
    case TARGET_X86_MSVC:     return 8;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 8;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_align_long_double(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 16;
    case TARGET_X86_MSVC:     return 8;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 8;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}

size_t get_size_long_double(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:  return 16;
    case TARGET_X86_MSVC:     return 8;
    case TARGET_X64_MSVC:     return 8;
    case TARGET_LCCU16:
    case TARGET_CCU8:         return 8;
    case TARGET_CATALINA:     return 4;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}


enum type_specifier_flags get_wchar_type_specifier(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return (TYPE_SPECIFIER_INT);
        break;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_SHORT);
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR);
        break;

    case TARGET_CATALINA:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_CHAR);
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}


enum type_specifier_flags get_intN_type_specifier(enum target target, int nbits)
{
    assert(nbits >= 8);

    switch (target)
    {
    case TARGET_X86_X64_GCC:
        if (nbits == 8) return TYPE_SPECIFIER_CHAR;
        if (nbits == 16) return TYPE_SPECIFIER_SHORT;
        if (nbits == 32) return TYPE_SPECIFIER_INT;
        if (nbits == 64) return TYPE_SPECIFIER_LONG;
        if (nbits == 128) return TYPE_SPECIFIER_LONG_LONG;
        break;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        if (nbits == 8) return TYPE_SPECIFIER_CHAR;
        if (nbits == 16) return TYPE_SPECIFIER_SHORT;
        if (nbits == 32) return TYPE_SPECIFIER_INT;
        if (nbits == 64) return TYPE_SPECIFIER_LONG_LONG;
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        if (nbits == 8) return TYPE_SPECIFIER_CHAR;
        if (nbits == 16) return TYPE_SPECIFIER_INT;
        if (nbits == 32) return TYPE_SPECIFIER_LONG;
        if (nbits == 64) return TYPE_SPECIFIER_LONG_LONG;
        break;
    case TARGET_CATALINA:
        if (nbits == 8) return TYPE_SPECIFIER_CHAR;
        if (nbits == 16) return TYPE_SPECIFIER_SHORT;
        if (nbits == 32) return TYPE_SPECIFIER_INT;
        if (nbits == 64) return TYPE_SPECIFIER_LONG_LONG;
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");

    assert(false);
    return TYPE_SPECIFIER_LONG_LONG;
}

enum type_specifier_flags get_ptrdiff_t_specifier(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return (TYPE_SPECIFIER_LONG);

    case TARGET_X86_MSVC:
        return (TYPE_SPECIFIER_INT);

    case TARGET_X64_MSVC:
        return (TYPE_SPECIFIER_LONG_LONG);

    case TARGET_LCCU16:
    case TARGET_CCU8:
        return (TYPE_SPECIFIER_INT);

    case TARGET_CATALINA:
        return (TYPE_SPECIFIER_INT);
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;

}

enum type_specifier_flags get_size_t_specifier(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG);

    case TARGET_X86_MSVC:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT);

    case TARGET_X64_MSVC:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_LONG_LONG);

    case TARGET_LCCU16:
    case TARGET_CCU8:
        return (TYPE_SPECIFIER_INT);

    case TARGET_CATALINA:
        return (TYPE_SPECIFIER_UNSIGNED | TYPE_SPECIFIER_INT);

    }
    static_assert(NUMBER_OF_TARGETS == 6, "add your new target here");
    assert(false);
    return 0;
}


bool is_automatic_variable(enum storage_class_specifier_flags f)
{
    if (f & STORAGE_SPECIFIER_EXTERN)
        return false;

    if (f & STORAGE_SPECIFIER_STATIC)
        return false;

    if (f & STORAGE_SPECIFIER_PARAMETER)
        return true;

    if (f & STORAGE_SPECIFIER_BLOCK_SCOPE)
        return true;

    return false;
}

void print_item(struct osstream* ss, bool* first, const char* item)
{
    if (!(*first))
        ss_fprintf(ss, " ");
    ss_fprintf(ss, "%s", item);
    *first = false;

}

bool print_type_alignment_flags(struct osstream* ss, bool* first, enum alignment_specifier_flags flags, enum target target)
{
    switch (target)
    {
    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        if (flags & ALIGNMENT_SPECIFIER_8_FLAGS)
            print_item(ss, first, "__declspec(align(8))");
        if (flags & ALIGNMENT_SPECIFIER_16_FLAGS)
            print_item(ss, first, "__declspec(align(16))");
        if (flags & ALIGNMENT_SPECIFIER_32_FLAGS)
            print_item(ss, first, "__declspec(align(32))");
        if (flags & ALIGNMENT_SPECIFIER_64_FLAGS)
            print_item(ss, first, "__declspec(align(64))");
        if (flags & ALIGNMENT_SPECIFIER_128_FLAGS)
            print_item(ss, first, "__declspec(align(128))");
        break;

    case TARGET_X86_X64_GCC:
        if (flags & ALIGNMENT_SPECIFIER_8_FLAGS)
            print_item(ss, first, "__attribute__((aligned(8)))");
        if (flags & ALIGNMENT_SPECIFIER_16_FLAGS)
            print_item(ss, first, "__attribute__((aligned(16)))");
        if (flags & ALIGNMENT_SPECIFIER_32_FLAGS)
            print_item(ss, first, "__attribute__((aligned(32)))");
        if (flags & ALIGNMENT_SPECIFIER_64_FLAGS)
            print_item(ss, first, "__attribute__((aligned(64)))");
        if (flags & ALIGNMENT_SPECIFIER_128_FLAGS)
            print_item(ss, first, "__attribute__((aligned(128)))");
        break;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        if (flags & ALIGNMENT_SPECIFIER_8_FLAGS)
            print_item(ss, first, "/*alignas(8)*/");
        if (flags & ALIGNMENT_SPECIFIER_16_FLAGS)
            print_item(ss, first, "/*alignas(16)*/");
        if (flags & ALIGNMENT_SPECIFIER_32_FLAGS)
            print_item(ss, first, "/*alignas(32)*/");
        if (flags & ALIGNMENT_SPECIFIER_64_FLAGS)
            print_item(ss, first, "/*alignas(64)*/");
        if (flags & ALIGNMENT_SPECIFIER_128_FLAGS)
            print_item(ss, first, "/*alignas(128)*/");
        break;

    case TARGET_CATALINA:
        if (flags & ALIGNMENT_SPECIFIER_8_FLAGS)
            print_item(ss, first, "__attribute__((aligned(8)))");
        if (flags & ALIGNMENT_SPECIFIER_16_FLAGS)
            print_item(ss, first, "__attribute__((aligned(16)))");
        if (flags & ALIGNMENT_SPECIFIER_32_FLAGS)
            print_item(ss, first, "__attribute__((aligned(32)))");
        if (flags & ALIGNMENT_SPECIFIER_64_FLAGS)
            print_item(ss, first, "__attribute__((aligned(32)))");
        if (flags & ALIGNMENT_SPECIFIER_128_FLAGS)
            print_item(ss, first, "__attribute__((aligned(32)))");
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "does this compiler have something similar of alignas?");

    return *first;
}

bool print_type_specifier_flags(struct osstream* ss, bool* first, enum type_specifier_flags e_type_specifier_flags)
{
    if (e_type_specifier_flags & TYPE_SPECIFIER_VOID)
        print_item(ss, first, "void");

    if (e_type_specifier_flags & TYPE_SPECIFIER_SIGNED)
        print_item(ss, first, "signed");

    if (e_type_specifier_flags & TYPE_SPECIFIER_UNSIGNED)
        print_item(ss, first, "unsigned");

    if (e_type_specifier_flags & TYPE_SPECIFIER_INT)
        print_item(ss, first, "int");

    if (e_type_specifier_flags & TYPE_SPECIFIER_SHORT)
        print_item(ss, first, "short");

    if (e_type_specifier_flags & TYPE_SPECIFIER_LONG)
        print_item(ss, first, "long");

    if (e_type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        print_item(ss, first, "long long");

    if (e_type_specifier_flags & TYPE_SPECIFIER_CHAR)
        print_item(ss, first, "char");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        print_item(ss, first, "double");

    if (e_type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        print_item(ss, first, "float");

    if (e_type_specifier_flags & TYPE_SPECIFIER_BOOL)
        print_item(ss, first, "_Bool");

    if (e_type_specifier_flags & TYPE_SPECIFIER_COMPLEX)
        print_item(ss, first, "_Complex");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL32)
        print_item(ss, first, "_Decimal32");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL64)
        print_item(ss, first, "_Decimal64");

    if (e_type_specifier_flags & TYPE_SPECIFIER_DECIMAL128)
        print_item(ss, first, "_Decimal128");

    if (e_type_specifier_flags & TYPE_SPECIFIER_BOOL)
        print_item(ss, first, "bool");

    if (e_type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
        print_item(ss, first, "nullptr_t");

    if (e_type_specifier_flags & TYPE_SPECIFIER_GCC__BUILTIN_VA_LIST)
        print_item(ss, first, "__builtin_va_list");



    return *first;
}

void print_type_qualifier_flags(struct osstream* ss, bool* first, enum type_qualifier_flags e_type_qualifier_flags)
{

    if (e_type_qualifier_flags & TYPE_QUALIFIER_CONST)
        print_item(ss, first, "const");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_RESTRICT)
        print_item(ss, first, "restrict");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_VOLATILE)
        print_item(ss, first, "volatile");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_OWNER)
        print_item(ss, first, "_Owner");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_DTOR)
        print_item(ss, first, "_Dtor");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_VIEW)
        print_item(ss, first, "_View");

    if (e_type_qualifier_flags & TYPE_QUALIFIER_OPT)
        print_item(ss, first, "_Opt");

}

void print_msvc_declspec(struct osstream* ss, bool* first, enum msvc_declspec_flags  msvc_declspec_flags)
{
    /*
       The objective is to print only what changes code generation / link
    */
    if (msvc_declspec_flags & MSVC_DECLSPEC_ALIGN_8_FLAG)
    {
        print_item(ss, first, "__declspec(align(8))");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_ALIGN_16_FLAG)
    {
        print_item(ss, first, "__declspec(align(16))");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_ALIGN_32_FLAG)
    {
        print_item(ss, first, "__declspec(align(32))");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_ALIGN_64_FLAG)
    {
        print_item(ss, first, "__declspec(align(64))");
    }

    if (msvc_declspec_flags & MSVC_DECLSPEC_ALLOCATE_FLAG)
    {
        print_item(ss, first, "__declspec(allocate)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_ALLOCATOR_FLAG)
    {
        print_item(ss, first, "__declspec(allocator)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_APPDOMAIN_FLAG)
    {

    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_CODE_SEG_FLAG)
    {

    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_DEPRECATED_FLAG)
    {

    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_DLLIMPORT_FLAG)
    {
        print_item(ss, first, "__declspec(dllimport)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_DLLEXPORT_FLAG)
    {
        print_item(ss, first, "__declspec(dllexport)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_EMPTY_BASES_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_HYBRID_PATCHABLE_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_JITINTRINSIC_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NAKED_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NOALIAS_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NOINLINE_FLAG)
    {
        print_item(ss, first, "__declspec(noinline)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NORETURN_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NOTHROW_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NOVTABLE_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_NO_SANITIZE_ADDRESS_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_PROCESS_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_PROPERTY_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_RESTRICT_FLAG)
    {
        print_item(ss, first, "__declspec(restrict)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_SAFEBUFFERS_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_SELECTANY_FLAG)
    {
        print_item(ss, first, "__declspec(selectany)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_SPECTRE_FLAG)
    {
        print_item(ss, first, "__declspec(spectre(nomitigation))");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_THREAD_FLAG)
    {
        print_item(ss, first, "__declspec(thread)");
    }
    if (msvc_declspec_flags & MSVC_DECLSPEC_UUID_FLAG)
    {
        //print_item(ss, first, "__declspec(selectany)");
    }
}

void print_type_qualifier_specifiers(struct osstream* ss, const struct type* type, enum target target)
{
    bool first = true;
    print_type_qualifier_flags(ss, &first, type->type_qualifier_flags);

    if (type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
    {
        assert(type->struct_or_union_specifier != NULL);
        print_item(ss, &first, "struct ");
        ss_fprintf(ss, "%s", type->struct_or_union_specifier->tag_name);
    }
    else if (type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        assert(type->enum_specifier != NULL);
        print_item(ss, &first, "enum ");
        if (type->enum_specifier->tag_token)
            ss_fprintf(ss, "%s", type->enum_specifier->tag_token->lexeme);

    }
    else if (type->type_specifier_flags & TYPE_SPECIFIER_TYPEDEF)
    {
        assert(false);
    }
    else
    {
        print_type_alignment_flags(ss, &first, type->alignment_specifier_flags, target);
        print_msvc_declspec(ss, &first, type->msvc_declspec_flags);
        print_type_specifier_flags(ss, &first, type->type_specifier_flags);
    }
}

void type_integer_promotion(struct type* a)
{
    //assert(type_is_integer(a));

    if ((a->type_specifier_flags & TYPE_SPECIFIER_BOOL) ||
        (a->type_specifier_flags & TYPE_SPECIFIER_CHAR) ||
        (a->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        )
    {
        a->type_specifier_flags = (TYPE_SPECIFIER_INT);
    }
}

void type_add_const(struct type* p_type)
{
    p_type->type_qualifier_flags |= TYPE_QUALIFIER_CONST;
}

void type_remove_qualifiers(struct type* p_type)
{
    p_type->type_qualifier_flags = 0;
}

struct type type_lvalue_conversion(const struct type* p_type, bool nullchecks_enabled)
{

    enum type_category category = type_get_category(p_type);
    switch (category)
    {
    case TYPE_CATEGORY_FUNCTION:
    {
        /*
           "function returning type" is converted to an expression that has type
           "pointer to function returning type".
        */
        struct type t = type_add_pointer(p_type, nullchecks_enabled);
        t.type_qualifier_flags &= ~TYPE_QUALIFIER_OPT;
        t.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;
        t.category = t.category;
        return t;
    }
    break;
    case TYPE_CATEGORY_ARRAY:
    {
        /*
          An expression that has type "array of type" is converted
          to an expression with type "pointer to type" that points to the initial element
          of the array object and s not an lvalue.
          If the array object has register storage class, the behavior is undefined.
        */
        struct type t = get_array_item_type(p_type);
        struct type t2 = type_add_pointer(&t, nullchecks_enabled);


        type_remove_qualifiers(&t2);
        /*
        int g(const int a[const 20]) {
            // in this function, a has type const int* const (const pointer to const int)
            }
        */
        type_destroy(&t);
        t2.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;
        return t2;
    }
    break;
    case TYPE_CATEGORY_POINTER:
        break;
    case TYPE_CATEGORY_ITSELF:
    default:
        break;
    }

    struct type t = type_dup(p_type);
    type_remove_qualifiers(&t);
    t.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;

    t.category = type_get_category(&t);

    return t;
}

struct type type_convert_to(const struct type* p_type, enum language_version target)
{
    /*
    * Convert types to previous standard format
    */

    if (target < LANGUAGE_C23 && type_is_nullptr_t(p_type))
    {

        struct type t = make_void_ptr_type();
        assert(t.name_opt == NULL);
        if (p_type->name_opt)
        {
            t.name_opt = strdup(p_type->name_opt);
        }
        return t;
    }


    return type_dup(p_type);
}

void print_type_core(struct osstream* ss, const struct type* p_type, bool onlydeclarator, bool printname, enum target target)
{
    const struct type* _Opt p = p_type;

    while (p)
    {
        if (onlydeclarator && p->next == NULL)
            break;

        switch (p->category)
        {
        case TYPE_CATEGORY_ITSELF:
        {
            struct osstream local = { 0 };
            bool first = true;

            print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);

            if (p->struct_or_union_specifier)
            {
                ss_fprintf(&local, "struct %s", p->struct_or_union_specifier->tag_name);
            }
            else if (p->enum_specifier)
            {
                if (p->enum_specifier->tag_token)
                {
                    ss_fprintf(&local, "enum %s", p->enum_specifier->tag_token->lexeme);
                }
                else
                {
                    ss_fprintf(&local, "enum ");
                }
            }
            else
            {
                print_type_alignment_flags(&local, &first, p->alignment_specifier_flags, target);
                print_msvc_declspec(&local, &first, p->msvc_declspec_flags);
                print_type_specifier_flags(&local, &first, p->type_specifier_flags);
            }



            if (printname && p->name_opt)
            {
                if (first)
                {
                    ss_fprintf(ss, " ");
                    first = false;
                }
                ss_fprintf(ss, "%s", p->name_opt);
            }

            struct osstream local2 = { 0 };
            if (ss->size > 0)
                ss_fprintf(&local2, "%s %s", local.c_str, ss->c_str);
            else
                ss_fprintf(&local2, "%s", local.c_str);

            ss_swap(ss, &local2);
            ss_close(&local);
            ss_close(&local2);
        }
        break;
        case TYPE_CATEGORY_ARRAY:


            if (printname && p->name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                ss_fprintf(ss, "%s", p->name_opt);
            }

            ss_fprintf(ss, "[");

            bool b = true;
            if (p->has_static_array_size)
            {
                ss_fprintf(ss, "static");
                b = false;
            }

            print_type_qualifier_flags(ss, &b, p->type_qualifier_flags);

            if (p->num_of_elements > 0)
            {
                if (!b)
                    ss_fprintf(ss, " ");

                ss_fprintf(ss, "%d", p->num_of_elements);
            }
            ss_fprintf(ss, "]");

            break;
        case TYPE_CATEGORY_FUNCTION:

            if (printname && p->name_opt)
            {
                //if (first)
                //{
                  //  ss_fprintf(ss, " ");
                    //first = false;
                //}
                ss_fprintf(ss, "%s", p->name_opt);
            }
            ss_fprintf(ss, "(");

            struct param* _Opt pa = p->params.head;

            while (pa)
            {
                struct osstream sslocal = { 0 };
                print_type(&sslocal, &pa->type, target);
                ss_fprintf(ss, "%s", sslocal.c_str);
                if (pa->next)
                    ss_fprintf(ss, ",");
                ss_close(&sslocal);
                pa = pa->next;
            }
            ss_fprintf(ss, ")");
            break;

        case TYPE_CATEGORY_POINTER:
        {
            struct osstream local = { 0 };
            if (p->next && (
                (p->next->category == TYPE_CATEGORY_FUNCTION ||
                    p->next->category == TYPE_CATEGORY_ARRAY)))
            {
                ss_fprintf(&local, "(");
            }

            ss_fprintf(&local, "*");
            bool first = false;
            print_type_qualifier_flags(&local, &first, p->type_qualifier_flags);

            if (printname && p->name_opt)
            {
                if (!first)
                {
                    ss_fprintf(ss, " ");
                }
                ss_fprintf(ss, "%s", p->name_opt);
                first = false;
            }

            if (ss->c_str)
                ss_fprintf(&local, "%s", ss->c_str);

            if (p->next &&
                (p->next->category == TYPE_CATEGORY_FUNCTION ||
                    p->next->category == TYPE_CATEGORY_ARRAY))
            {
                ss_fprintf(&local, ")", ss->c_str);
            }

            ss_swap(ss, &local);
            ss_close(&local);
        }
        break;
        }

        p = p->next;

    }
}

void print_type(struct osstream* ss, const struct type* p_type, enum target target)
{
    print_type_core(ss, p_type, false, true, target);
}

void print_type_no_names(struct osstream* ss, const struct type* p_type, enum target target)
{
    print_type_core(ss, p_type, false, false, target);
}

void print_type_declarator(struct osstream* ss, const struct type* p_type, enum target target)
{
    print_type_core(ss, p_type, true, true, target);
}

void type_print(const struct type* a, enum target target)
{
    struct osstream ss = { 0 };
    print_type(&ss, a, target);
    printf("%s", ss.c_str);
    ss_close(&ss);
}

void type_println(const struct type* a, enum target target)
{
    type_print(a, target);
    puts("\n");
}

enum type_category type_get_category(const struct type* p_type)
{
    return p_type->category;
}

void param_list_add(struct param_list* list, struct param* _Owner p_item)
{
    if (list->head == NULL)
    {
        list->head = p_item;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void param_list_destroy(_Dtor struct param_list* p)
{
    struct param* _Owner _Opt item = p->head;
    while (item)
    {
        struct param* _Owner _Opt next = item->next;
        type_destroy(&item->type);
        free(item);
        item = next;
    }
}

void type_destroy_one(_Opt _Dtor struct type* p_type)
{
    free((void* _Owner)p_type->name_opt);
    param_list_destroy(&p_type->params);
    assert(p_type->next == NULL);
}

void type_destroy(_Opt _Dtor struct type* p_type)
{
    free((void* _Owner)p_type->name_opt);
    param_list_destroy(&p_type->params);

    struct type* _Owner _Opt item = p_type->next;
    while (item)
    {
        struct type* _Owner _Opt next = item->next;
        item->next = NULL;
        type_destroy_one(item);
        free(item);
        item = next;
    }

}

void type_delete(struct type* _Owner _Opt p_type)
{
    if (p_type)
    {
        type_destroy(p_type);
        free(p_type);
    }
}

bool type_has_attribute(const struct type* p_type, enum attribute_flags attributes)
{
    if (p_type->attributes_flags & attributes)
    {
        /*like
          [[maybe_unused]] int i;
        */
        return true;
    }

    struct attribute_specifier_sequence* _Opt p_attribute_specifier_sequence = NULL;

    if (p_type->struct_or_union_specifier)
    {
        /*like
          struct [[maybe_unused]] X { }
          struct X x;
        */
        p_attribute_specifier_sequence = p_type->struct_or_union_specifier->attribute_specifier_sequence_opt;

        struct struct_or_union_specifier* _Opt p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_attribute_specifier_sequence == NULL && p_complete)
        {
            p_attribute_specifier_sequence = p_complete->attribute_specifier_sequence_opt;
        }
    }
    else if (p_type->enum_specifier)
    {
        const struct enum_specifier* _Opt p_complete_enum_specifier =
            get_complete_enum_specifier(p_type->enum_specifier);

        p_attribute_specifier_sequence = p_type->enum_specifier->attribute_specifier_sequence_opt;

        if (p_attribute_specifier_sequence == NULL && p_complete_enum_specifier)
        {
            p_attribute_specifier_sequence = p_complete_enum_specifier->attribute_specifier_sequence_opt;
        }
    }

    if (p_attribute_specifier_sequence &&
        p_attribute_specifier_sequence->attributes_flags & attributes)
    {
        return true;
    }

    return false;
}

bool type_is_maybe_unused(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_MAYBE_UNUSED);
}

bool type_is_deprecated(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_DEPRECATED);
}

bool type_is_nodiscard(const struct type* p_type)
{
    return type_has_attribute(p_type, STD_ATTRIBUTE_NODISCARD);
}

bool type_is_array(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ARRAY;
}

bool type_is_owner_or_pointer_to_dtor(const struct type* p_type)
{
    if (type_is_pointed_dtor(p_type))
        return true;

    if (type_is_owner(p_type))
    {
        return true;
    }
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_DTOR;
}

bool type_is_pointer_to_owner(const struct type* p_type)
{
    if (p_type->next == NULL)
        return false;

    return type_is_owner(p_type->next);
}

bool type_is_dtor(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_DTOR;
}

bool type_is_pointed_dtor(const struct type* p_type)
{
    if (!type_is_pointer(p_type))
        return false;

    assert(p_type->next != NULL);

    return type_is_dtor(p_type->next);
}

bool type_is_owner(const struct type* p_type)
{
    if (p_type->struct_or_union_specifier)
    {
        if (p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW)
            return false;

        struct struct_or_union_specifier* _Opt p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_complete && p_complete->is_owner)
        {
            //The objective here is fix a type later.
            /*
             struct X;
             struct X f(); //X is _Owner?
             struct X { char * _Owner p; };
             int main()
             {
               struct X x = 1 ? f() : f();
             }
            */

            return true;
        }
    }

    return p_type->type_qualifier_flags & TYPE_QUALIFIER_OWNER;
}

bool type_is_opt(const struct type* p_type, bool nullable_enabled)
{
    if (nullable_enabled)
    {
        return p_type->type_qualifier_flags & TYPE_QUALIFIER_OPT;
    }

    //If  nullable_enabled is disabled then all pointers are nullable
    return true;
}

bool type_is_view(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_VIEW;
}

bool type_is_ctor(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_CTOR;
}

bool type_is_const(const struct type* p_type)
{
    return p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST;
}

bool type_is_constexpr(const struct type* p_type)
{
    return (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR);
}

bool type_is_const_or_constexpr(const struct type* p_type)
{
    return (p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST) ||
        (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_CONSTEXPR);
}

bool type_is_pointer_to_const(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next)
        {
            return p_type->next->type_qualifier_flags & TYPE_QUALIFIER_CONST;
        }
    }
    return false;
}

bool type_is_void_ptr(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next)
        {
            return p_type->next->type_specifier_flags & TYPE_SPECIFIER_VOID;
        }
    }
    return false;
}

bool type_is_void(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_ITSELF)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_VOID;
    }

    return false;
}

bool type_is_nullptr_t(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_ITSELF)
    {
        return p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T;
    }

    return false;
}

bool type_is_pointer_to_out(const struct type* p_type)
{
    if (p_type->next == NULL)
        return false;

    if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        return p_type->next->type_qualifier_flags & TYPE_QUALIFIER_CTOR;
    }
    return false;
}

bool type_is_pointer(const struct type* p_type)
{
    return p_type->category == TYPE_CATEGORY_POINTER;
}

bool type_is_essential_bool(const struct type* p_type)
{
    return p_type->attributes_flags & CAKE_HIDDEN_ATTRIBUTE_LIKE_BOOL;
}
bool type_is_essential_char(const struct type* p_type)
{
    return p_type->attributes_flags & CAKE_HIDDEN_ATTRIBUTE_INT_LIKE_CHAR;
}

bool type_is_enum(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM;
}

bool type_is_struct_or_union(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION;
}

bool type_is_union(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    if (p_type->struct_or_union_specifier == NULL)
        return false;

    return p_type->struct_or_union_specifier->first_token->type == TK_KEYWORD_UNION;
}

/*
  The three types
  char, signed char, and unsigned char
  are collectively called the character types.
*/
bool type_is_character(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_vla(const struct type* p_type)
{
    const struct type* _Opt it = p_type;

    while (it && type_is_array(it))
    {
        if (it->array_num_elements_expression)
        {
            if (!object_has_constant_value(&it->array_num_elements_expression->object))
            {
                // int a[7][n]
                //if any of the array is not constant then it is vla
                return true;
            }
        }
        it = it->next;
    }
    return false;
}

bool type_is_decimal128(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_DECIMAL128;
}
bool type_is_decimal64(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_DECIMAL64;
}
bool type_is_decimal32(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_DECIMAL32;
}
bool type_is_long_double(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return  false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            return true;
        }

    }
    return false;
}

bool type_is_double(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return  false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        if (!(p_type->type_specifier_flags & TYPE_SPECIFIER_LONG))
        {
            return true;
        }

    }
    return false;
}

bool type_is_int(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return  false;

    if ((p_type->type_specifier_flags == (TYPE_SPECIFIER_INT | TYPE_SPECIFIER_SIGNED)) ||
        (p_type->type_specifier_flags == TYPE_SPECIFIER_INT))
    {
        return true;
    }
    return false;
}

bool type_is_unsigned_int(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return  false;

    if (p_type->type_specifier_flags == (TYPE_SPECIFIER_INT | TYPE_SPECIFIER_UNSIGNED))
    {
        return true;
    }
    return false;
}


bool type_is_float(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return  false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
    {
        return true;
    }
    return false;
}


bool type_is_bool(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL;
}

/*
 There are three standard floating types, designated as
 float, double, and long double.

 There are three decimal floating types, designated as _Decimal32, _Decimal64, and _Decimal128.
*/
bool type_is_floating_point(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_DOUBLE |
            TYPE_SPECIFIER_FLOAT);
}

bool type_is_unsigned_integer(const struct type* p_type)
{
    if (type_is_integer(p_type) &&
        (p_type->type_specifier_flags & TYPE_SPECIFIER_UNSIGNED))
    {
        return true;
    }

    return false;
}

bool type_is_signed_integer(const struct type* p_type)
{
    if (type_is_integer(p_type) &&
        !(p_type->type_specifier_flags & TYPE_SPECIFIER_UNSIGNED))
    {
        return true;
    }

    return false;
}

bool type_is_array_of_char(const struct type* p_type)
{
    if (p_type->category != TYPE_CATEGORY_ARRAY)
        return false;

    assert(p_type->next != NULL);
    return p_type->next->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

bool type_is_char(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    return p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR;
}

/*
  The type char, the signed and unsigned integer types,
  and the enumerated types
  are collectively  called integer types.
*/
bool type_is_integer(const struct type* p_type)
{
    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        /*we cannot check long without check double*/
        //long double
        return false;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        return true;
    }

    return p_type->type_specifier_flags &
        (TYPE_SPECIFIER_CHAR |
            TYPE_SPECIFIER_SHORT |
            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_INT |
            TYPE_SPECIFIER_LONG |
            TYPE_SPECIFIER_SIGNED |
            TYPE_SPECIFIER_UNSIGNED |
            TYPE_SPECIFIER_LONG_LONG |
            TYPE_SPECIFIER_BOOL);
}

/*
* Integer and floating types are collectively called arithmetic types.
*/
bool type_is_arithmetic(const struct type* p_type)
{
    return type_is_integer(p_type) || type_is_floating_point(p_type);
}

/*
 Arithmetic types, pointer types, and the nullptr_t type are collectively
 called scalar types.
*/
bool type_is_scalar(const struct type* p_type)
{
    //TODO we need two concepts...is_scalar on real type or is_scalar after lvalue converison

    if (type_is_arithmetic(p_type))
        return true;

    if (type_is_pointer(p_type))
        return true;

    if (type_get_category(p_type) != TYPE_CATEGORY_ITSELF)
        return false;


    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        return true;
    if (p_type->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T)
        return true;

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        return true;

    return false;
}


const struct param_list* _Opt type_get_func_or_func_ptr_params(const struct type* p_type)
{
    if (p_type->category == TYPE_CATEGORY_FUNCTION)
    {
        return &p_type->params;
    }
    else if (p_type->category == TYPE_CATEGORY_POINTER)
    {
        if (p_type->next &&
            p_type->next->category == TYPE_CATEGORY_FUNCTION)
        {
            return &p_type->next->params;
        }
    }
    return NULL;
}

void check_ownership_qualifiers_of_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num)
{
    //            _Owner     _Dtor  _View parameter
    // _Owner      OK                   OK
    // _Dtor  X         OK         OK
    // _View       X (NULL)  X          OK

    const bool paramer_is_obj_owner = type_is_pointed_dtor(paramer_type);
    const bool paramer_is_owner = type_is_owner(paramer_type);
    const bool paramer_is_view = !paramer_is_obj_owner && !paramer_is_owner;

    const struct type* const argument_type = &current_argument->expression->type;
    const bool argument_is_owner = type_is_owner(&current_argument->expression->type);
    const bool argument_is_obj_owner = type_is_pointed_dtor(&current_argument->expression->type);
    const bool argument_is_view = !argument_is_owner && !argument_is_obj_owner;

    if (argument_is_owner && paramer_is_owner)
    {
        //ok
    }
    else if (argument_is_owner && paramer_is_obj_owner)
    {
        //ok
    }
    else if (argument_is_owner && paramer_is_view)
    {
        //ok
        if (current_argument->expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_diagnostic(W_OWNERSHIP_USING_TEMPORARY_OWNER,
                ctx,
                current_argument->expression->first_token, NULL,
                "passing a temporary owner to a view");
        }

    }////////////////////////////////////////////////////////////
    else if (argument_is_obj_owner && paramer_is_owner)
    {
        compiler_diagnostic(W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
            ctx,
            current_argument->expression->first_token, NULL,
            "cannot move _Dtor to _Owner");
    }
    else if (argument_is_obj_owner && paramer_is_obj_owner)
    {
        //ok
    }
    else if (argument_is_obj_owner && paramer_is_view)
    {
        //ok
        //ok
        if (current_argument->expression->type.storage_class_specifier_flags & STORAGE_SPECIFIER_FUNCTION_RETURN)
        {
            compiler_diagnostic(W_OWNERSHIP_USING_TEMPORARY_OWNER,
                ctx,
                current_argument->expression->first_token, NULL,
                "passing a temporary owner to a view");
        }


    }///////////////////////////////////////////////////////////////
    else if (argument_is_view && paramer_is_owner)
    {
        if (!expression_is_null_pointer_constant(current_argument->expression))
        {
            compiler_diagnostic(W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                ctx,
                current_argument->expression->first_token, NULL,
                "passing a _View argument to a _Owner parameter");
        }
    }
    else if (argument_is_view && paramer_is_obj_owner)
    {
        //check if the contented of pointer is _Owner.
        if (type_is_pointer(argument_type))
        {
            struct type t2 = type_remove_pointer(argument_type);
            if (!type_is_owner(&t2))
            {

                compiler_diagnostic(W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                    ctx,
                    current_argument->expression->first_token, NULL,
                    "pointed object is not _Owner");

            }
            else
            {
                //pointer object is _Owner
                if (!argument_type->address_of)
                {
                    //we need something created with address of.
                    compiler_diagnostic(W_MUST_USE_ADDRESSOF,
                        ctx,
                        current_argument->expression->first_token, NULL,
                        "_Dtor pointer must be created using address of operator &");
                }
            }

            type_destroy(&t2);
        }
        else
        {
            if (!expression_is_null_pointer_constant(current_argument->expression))
            {
                compiler_diagnostic(W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER,
                    ctx,
                    current_argument->expression->first_token, NULL,
                    "passing a _View argument to a _Dtor parameter");
            }
        }

    }
    else if (argument_is_view && paramer_is_view)
    {
        //ok
    }///////////////////////////////////////////////////////////////
}

void check_argument_and_parameter(struct parser_ctx* ctx,
    struct argument_expression* current_argument,
    struct type* paramer_type,
    int param_num)
{
    // TODO use assignment check for everthing..

    if (type_is_owner_or_pointer_to_dtor(paramer_type))
    {
        if (type_is_pointed_dtor(paramer_type))
        {
            if (current_argument->expression->type.category == TYPE_CATEGORY_POINTER)
            {
                if (type_is_pointer(&current_argument->expression->type) &&
                    !type_is_pointer_to_owner(&current_argument->expression->type))
                {
                    compiler_diagnostic(W_OWNERSHIP_NOT_OWNER, ctx,
                        current_argument->expression->first_token, NULL,
                        "parameter %d requires a pointer to _Owner object",
                        param_num);
                }
            }
            else
            {
                compiler_diagnostic(W_OWNERSHIP_NOT_OWNER, ctx,
                    current_argument->expression->first_token, NULL,
                    "parameter %d requires a pointer to _Owner type",
                    param_num);
            }
        }
    }

    struct type* argument_type = &current_argument->expression->type;
    const bool is_null_pointer_constant = expression_is_null_pointer_constant(current_argument->expression);

    struct type parameter_type_converted = (type_is_array(paramer_type)) ?
        type_lvalue_conversion(paramer_type, ctx->options.null_checks_enabled) :
        type_dup(paramer_type);


    struct type argument_type_converted =
        expression_is_subjected_to_lvalue_conversion(current_argument->expression) ?
        type_lvalue_conversion(argument_type, ctx->options.null_checks_enabled) :
        type_dup(argument_type);

    /*
       less generic tests are first
    */
    if (type_is_enum(argument_type) && type_is_enum(paramer_type))
    {
        if (!type_is_same(argument_type, paramer_type, false))
        {
            compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx,
                current_argument->expression->first_token, NULL,
                " incompatible types at argument %d", param_num);
        }

        check_ownership_qualifiers_of_argument_and_parameter(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    if (type_is_arithmetic(argument_type) && type_is_arithmetic(paramer_type))
    {
        check_ownership_qualifiers_of_argument_and_parameter(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    if (is_null_pointer_constant && type_is_pointer(paramer_type))
    {
        //TODO void F(int * [[_Opt]] p)
        // F(0) when passing null we will check if the parameter
        //have the anotation [[_Opt]]

        /*can be converted to any type*/
        check_ownership_qualifiers_of_argument_and_parameter(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    if (is_null_pointer_constant && type_is_array(paramer_type))
    {
        compiler_diagnostic(W_FLOW_NON_NULL,
            ctx,
            current_argument->expression->first_token, NULL,
            " passing null as array");

        check_ownership_qualifiers_of_argument_and_parameter(ctx,
            current_argument,
            paramer_type,
            param_num);

        type_destroy(&parameter_type_converted);
        type_destroy(&argument_type_converted);

        return;
    }

    /*
       We have two pointers or pointer/array combination
    */
    if (type_is_pointer_or_array(argument_type) && type_is_pointer_or_array(paramer_type))
    {
        if (type_is_void_ptr(argument_type))
        {
            /*void pointer can be converted to any type*/
            check_ownership_qualifiers_of_argument_and_parameter(ctx,
                current_argument,
                paramer_type,
                param_num);

            type_destroy(&parameter_type_converted);
            type_destroy(&argument_type_converted);

            return;
        }

        if (type_is_void_ptr(paramer_type))
        {
            /*any pointer can be converted to void* */
            check_ownership_qualifiers_of_argument_and_parameter(ctx,
                current_argument,
                paramer_type,
                param_num);

            type_destroy(&parameter_type_converted);
            type_destroy(&argument_type_converted);

            return;
        }


        //TODO  lvalue

        if (type_is_array(paramer_type))
        {
            unsigned long long parameter_array_size = paramer_type->num_of_elements;
            if (type_is_array(argument_type))
            {
                unsigned long long argument_array_size = argument_type->num_of_elements;
                if (parameter_array_size != 0 &&
                    argument_array_size < parameter_array_size)
                {
                    compiler_diagnostic(C_ERROR_ARGUMENT_SIZE_SMALLER_THAN_PARAMETER_SIZE,
                        ctx,
                        current_argument->expression->first_token, NULL,
                        " argument of size [%d] is smaller than parameter of size [%d]", argument_array_size, parameter_array_size);
                }
            }
            else if (is_null_pointer_constant || type_is_nullptr_t(argument_type))
            {
                compiler_diagnostic(W_PASSING_NULL_AS_ARRAY,
                    ctx,
                    current_argument->expression->first_token, NULL,
                    " passing null as array");
            }
        }



        if (!type_is_same(&argument_type_converted, &parameter_type_converted, false))
        {
            type_print(&argument_type_converted, ctx->options.target);
            type_print(&parameter_type_converted, ctx->options.target);

            compiler_diagnostic(C_ERROR_INCOMPATIBLE_TYPES, ctx,
                current_argument->expression->first_token, NULL,
                " incompatible types at argument %d", param_num);
            //disabled for now util it works correctly
            //return false;
        }

        if (type_is_pointer(&argument_type_converted) && type_is_pointer(&parameter_type_converted))
        {
            //parameter pointer do non const
            //argument const.
            struct type argument_pointer_to = type_remove_pointer(&argument_type_converted);
            struct type parameter_pointer_to = type_remove_pointer(&parameter_type_converted);
            if (type_is_const(&argument_pointer_to) &&
                !type_is_const(&parameter_pointer_to) &&
                !type_is_owner_or_pointer_to_dtor(&parameter_pointer_to))
            {
                compiler_diagnostic(W_DISCARDED_QUALIFIERS, ctx,
                    current_argument->expression->first_token, NULL,
                    " discarding const at argument %d", param_num);
            }
            type_destroy(&argument_pointer_to);
            type_destroy(&parameter_pointer_to);
        }
        //return true;
    }

    //TODO
    //if (!type_is_same(paramer_type, &current_argument->expression->type, false))
    //{
    //    compiler_diagnostic(C1, ctx,
    //        current_argument->expression->first_token,
    //        " incompatible types at argument %d ", param_num);
    //}



    check_ownership_qualifiers_of_argument_and_parameter(ctx,
        current_argument,
        paramer_type,
        param_num);



    type_destroy(&argument_type_converted);
    type_destroy(&parameter_type_converted);
}



bool type_is_function(const struct type* p_type)
{
    return type_get_category(p_type) == TYPE_CATEGORY_FUNCTION;
}

bool type_is_function_or_function_pointer(const struct type* p_type)
{
    if (type_is_function(p_type))
        return true;

    if (type_is_pointer(p_type))
    {
        //TODO not optimized
        struct type t = type_remove_pointer(p_type);
        bool r = type_is_function(&t);
        type_destroy(&t);
        return r;
    }

    return false;
}

bool type_is_empty(const struct type* p_type)
{
    return p_type->category == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags == TYPE_SPECIFIER_NONE;
}

struct type type_add_pointer(const struct type* p_type, bool null_checks_enabled)
{
    struct type r = type_dup(p_type);
    try
    {
        //waiting test
        //if (type_is_empty(&r)) throw;

        struct type* _Owner _Opt p = calloc(1, sizeof(struct type));
        if (p == NULL) throw;

        *p = r;
        r = (struct type){ 0 };
        r.next = p;
        r.category = TYPE_CATEGORY_POINTER;


        r.storage_class_specifier_flags = p_type->storage_class_specifier_flags;
    }
    catch
    {
    }

    return r;
}

struct type type_remove_pointer(const struct type* p_type)
{
    struct type r = type_dup(p_type);
    if (!type_is_pointer(p_type))
    {
        return r;
    }

    if (r.next)
    {
        struct type next = *r.next;
        /*
          we have moved the contents of r.next, but we also need to delete it's memory
        */
        free(r.next);
        r.next = NULL;
        type_destroy_one(&r);
        r = next;
    }
    else
    {
        assert(false);
    }

    assert(p_type->next != NULL); //guaranteed by type_is_pointer

    r.storage_class_specifier_flags = p_type->next->storage_class_specifier_flags;
    r.type_qualifier_flags = p_type->next->type_qualifier_flags;

    return r;
}


struct type get_array_item_type(const struct type* p_type)
{
    struct type r = type_dup(p_type);

    if (r.next)
    {
        struct type r2 = *r.next;

        free(r.next);
        free((void* _Owner) r.name_opt);
        param_list_destroy(&r.params);
        return r2;
    }

    return r;
}

struct type type_param_array_to_pointer(const struct type* p_type, bool null_checks_enabled)
{
    assert(type_is_array(p_type));
    struct type t = get_array_item_type(p_type);
    struct type t2 = type_add_pointer(&t, null_checks_enabled);

    if (p_type->type_qualifier_flags & TYPE_QUALIFIER_CONST)
    {
        /*
         void F(int a[static const 5]) {
          static_assert((typeof(a)) == (int* const));
        }
        */
        t2.type_qualifier_flags |= TYPE_QUALIFIER_CONST;
    }

    type_destroy(&t);
    t2.storage_class_specifier_flags &= ~STORAGE_SPECIFIER_PARAMETER;

    return t2;
}

bool type_is_pointer_or_array(const struct type* p_type)
{
    const enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER ||
        category == TYPE_CATEGORY_ARRAY)
    {
        return true;
    }

    if (category == TYPE_CATEGORY_ITSELF &&
        p_type->type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
    {
        return true;
    }

    return false;
}


//See 6.3.1.1
int type_get_integer_rank(const struct type* p_type1)
{
    if (type_is_pointer_or_array(p_type1))
    {
        assert(false);
        return 40;
    }

    if (p_type1->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
    {
        return 80;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_NULLPTR_T))
    {
        return 50; //?
    }
    else if (p_type1->type_specifier_flags & TYPE_SPECIFIER_LONG)
    {
        return 50;
    }
    else if ((p_type1->type_specifier_flags & TYPE_SPECIFIER_INT) ||
        (p_type1->type_specifier_flags & TYPE_SPECIFIER_ENUM))
    {
        return 40;
    }
    else if (p_type1->type_specifier_flags & TYPE_SPECIFIER_SHORT)
    {
        return 30;
    }
    else if (p_type1->type_specifier_flags & TYPE_SPECIFIER_CHAR)
    {
        return 20;
    }
    else if (p_type1->type_specifier_flags & TYPE_SPECIFIER_BOOL)
    {
        return 10;
    }

    return 0;
}

struct type type_get_enum_underlying_type(const struct type* p)
{
    struct type r = type_make_int();
    //TODO
    return r;
}

struct type type_common(const struct type* p_type1, const struct type* p_type2, enum target target)
{
    //See 6.3.1.8 Usual arithmetic conversions


    /*
       First, if the type of either operand is _Decimal128,
       the other operand is converted to _Decimal128.
    */
    if (type_is_decimal128(p_type1))
    {
        return type_dup(p_type1);
    }

    if (type_is_decimal128(p_type2))
    {
        return type_dup(p_type2);
    }

    /*
      Otherwise, if the type of either operand is _Decimal64,
      the other operand is converted to _Decimal64
    */

    if (type_is_decimal64(p_type1))
    {
        return type_dup(p_type1);
    }

    if (type_is_decimal64(p_type2))
    {
        return type_dup(p_type2);
    }

    /*
      Otherwise, if the type of either operand is _Decimal32,
      the other operand is converted to _Decimal32.
    */
    if (type_is_decimal32(p_type1))
    {
        return type_dup(p_type1);
    }

    if (type_is_decimal32(p_type2))
    {
        return type_dup(p_type2);
    }

    /*
      Otherwise, if the corresponding real type of either operand is long double,
      the other operand is converted, without change of type domain, to a type whose
      corresponding real type is long double
    */
    if (type_is_long_double(p_type1))
    {
        return type_dup(p_type1);
    }

    if (type_is_long_double(p_type2))
    {
        return type_dup(p_type2);
    }

    /*
      Otherwise, if the corresponding real type of either operand is double,
      the other operand is converted, without change of type domain, to a type
      whose corresponding real type is double.
    */

    if (type_is_double(p_type1))
    {
        return type_dup(p_type1);
    }

    if (type_is_double(p_type2))
    {
        return type_dup(p_type2);
    }


    /*
      Otherwise, if the corresponding real type of either operand is float,
      the other operand is converted, without change of type domain,
      to a type whose corresponding real type is float
    */
    if (type_is_float(p_type1))
    {
        return type_dup(p_type1);
    }

    if (type_is_float(p_type2))
    {
        return type_dup(p_type2);
    }

    /*
     Otherwise, if any of the two types is an enumeration, it is converted to its underlying type.
    */
    struct type promoted_a = { 0 };
    struct type promoted_b = { 0 };


    if (type_is_enum(p_type1))
    {
        promoted_a = type_get_enum_underlying_type(p_type1);

    }
    else
    {
        promoted_a = type_dup(p_type1);
    }

    if (type_is_enum(p_type2))
    {
        promoted_b = type_get_enum_underlying_type(p_type2);
    }
    else
    {
        promoted_b = type_dup(p_type2);
    }

    /*
      Then, the integer promotions are performed on both operands. Next, the following rules are
      applied to the promoted operands
    */
    type_integer_promotion(&promoted_a);
    type_integer_promotion(&promoted_b);



    /*
      if both operands have the same type, then no further conversion is needed
    */
    if (type_is_same(&promoted_a, &promoted_b, false))
    {
        type_destroy(&promoted_b);
        return promoted_a;
    }

    /*
     Otherwise, if both operands have signed integer types or both have unsigned integer
     types, the operand with the type of lesser integer conversion rank is converted to the type
     of the operand with greater rank.
    */

    if (type_is_signed_integer(&promoted_a) == type_is_signed_integer(&promoted_b))
    {
        if (type_get_integer_rank(&promoted_a) > type_get_integer_rank(&promoted_b))
        {
            type_destroy(&promoted_b);
            return promoted_a;
        }

        type_destroy(&promoted_a);
        return promoted_b;
    }


    /*
     Otherwise, if the operand that has unsigned integer type has rank greater or equal to
     the rank of the type of the other operand, then the operand with signed integer type is
     converted to the type of the operand with unsigned integer type.
    */

    struct type* p_signed_promoted = type_is_signed_integer(&promoted_a) ? &promoted_a : &promoted_b;
    struct type* p_unsigned_promoted = type_is_unsigned_integer(&promoted_a) ? &promoted_a : &promoted_b;

    assert(p_signed_promoted != p_unsigned_promoted);

    if (type_get_integer_rank(p_unsigned_promoted) >= type_get_integer_rank(p_signed_promoted))
    {
        struct type r = { 0 };
        type_swap(&r, p_unsigned_promoted);
        type_destroy(&promoted_a);
        type_destroy(&promoted_b);
        return r;
    }

    /*
      Otherwise, if the type of the operand with signed integer type can represent all the values
      of the type of the operand with unsigned integer type, then the operand with unsigned
      integer type is converted to the type of the operand with signed integer type
    */

    size_t signed_promoted_sizeof = 0;
    if (type_get_sizeof(p_signed_promoted, &signed_promoted_sizeof, target) != 0)
    {
        assert(false);
    }

    size_t unsigned_promoted_sizeof = 0;
    if (type_get_sizeof(p_unsigned_promoted, &unsigned_promoted_sizeof, target) != 0)
    {
        assert(false);
    }

    if (signed_promoted_sizeof > unsigned_promoted_sizeof)
    {
        struct type r = { 0 };
        type_swap(&r, p_signed_promoted);
        type_destroy(&promoted_a);
        type_destroy(&promoted_b);
        return r;
    }

    /*
      Otherwise, both operands are converted to the unsigned integer type corresponding to
      the type of the operand with signed integer type
    */

    struct type r = { 0 };
    type_swap(&r, p_signed_promoted);
    r.type_specifier_flags |= TYPE_SPECIFIER_UNSIGNED;
    type_destroy(&promoted_a);
    type_destroy(&promoted_b);
    return r;
}

void type_set(struct type* a, const struct type* b)
{
    struct type t = type_dup(b);
    type_swap(&t, a);
    type_destroy(&t);
}

struct type type_dup(const struct type* p_type)
{
    try
    {
        struct type_list l = { 0 };
        const struct type* _Opt p = p_type;
        while (p)
        {
            struct type* _Owner _Opt p_new = calloc(1, sizeof(struct type));
            if (p_new == NULL)
            {
                type_list_destroy(&l);
                throw;
            }

            *p_new = *p;

            //actually I was not the _Owner of p_new->next
            static_set(p_new->next, "uninitialized");
            p_new->next = NULL;

            if (p->name_opt)
            {
                //actually p_new->name_opt was not mine..
                static_set(p_new->name_opt, "uninitialized");
                p_new->name_opt = strdup(p->name_opt);
            }

            if (p->category == TYPE_CATEGORY_FUNCTION)
            {
                //actually p_new->params.head  p_new->params.tail and was not mine..
                static_set(p_new->params.head, "uninitialized");
                p_new->params.head = NULL;
                static_set(p_new->params.tail, "uninitialized");
                p_new->params.tail = NULL;

                struct param* _Opt p_param = p->params.head;
                while (p_param)
                {
                    struct param* _Owner _Opt p_new_param = calloc(1, sizeof * p_new_param);
                    if (p_new_param == NULL)
                    {
                        type_list_destroy(&l);
                        type_delete(p_new);
                        throw;
                    }

                    p_new_param->type = type_dup(&p_param->type);

                    param_list_add(&p_new->params, p_new_param);
                    p_param = p_param->next;
                }
            }

            type_list_push_back(&l, p_new);
            p = p->next;
        }

        if (l.head == NULL)
            throw;

        struct type r = *l.head;
        /*
           we have moved the content of l.head
           but we also need to delete the memory
        */
        free(l.head);

        return r;
    }
    catch
    {
    }

    struct type empty = { 0 };
    return empty;
}

static enum sizeof_error get_offsetof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier,
    const char* member, size_t* sz, enum target target)
{
    enum sizeof_error sizeof_error = ESIZEOF_NONE;

    const bool is_union =
        (complete_struct_or_union_specifier->first_token->type == TK_KEYWORD_UNION);

    size_t size = 0;
    try
    {
        size_t maxalign = 0;

        struct member_declaration* _Opt d = complete_struct_or_union_specifier->member_declaration_list.head;
        while (d)
        {
            if (d->member_declarator_list_opt)
            {
                struct member_declarator* _Opt md = d->member_declarator_list_opt->head;
                while (md)
                {
                    size_t align = 1;

                    if (md->declarator)
                    {
                        assert(md->declarator->name_opt != NULL);

                        align = type_get_alignof(&md->declarator->type, target);

                        if (align > maxalign)
                        {
                            maxalign = align;
                        }
                        if (size % align != 0)
                        {
                            size += align - (size % align);
                        }

                        if (strcmp(md->declarator->name_opt->lexeme, member) == 0)
                        {
                            *sz = size;
                            return ESIZEOF_NONE;
                        }

                        size_t item_size = 0;
                        sizeof_error = type_get_sizeof(&md->declarator->type, &item_size, target);
                        if (sizeof_error != 0)
                            throw;

                        if (is_union)
                        {
                            if (item_size > size)
                                size = item_size;
                        }
                        else
                        {
                            size += item_size;
                        }
                    }
                    else
                    {
                        sizeof_error = ESIZEOF_INCOMPLETE;
                        throw;
                    }

                    md = md->next;
                }
            }
            else if (d->specifier_qualifier_list)
            {
                if (d->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct type t = { 0 };
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                    size_t align = type_get_alignof(&t, target);

                    if (align > maxalign)
                    {
                        maxalign = align;
                    }
                    if (size % align != 0)
                    {
                        size += align - (size % align);
                    }
                    size_t item_size = 0;

                    sizeof_error = type_get_sizeof(&t, &item_size, target);
                    if (sizeof_error != 0)
                        throw;

                    if (is_union)
                    {
                        if (item_size > size)
                            size = item_size;
                    }
                    else
                    {
                        //TODO overflow
                        size += item_size;
                    }
                    type_destroy(&t);
                }
                else
                {
                    sizeof_error = ESIZEOF_INCOMPLETE;
                    throw;
                }
            }

            d = d->next;
        }
        if (maxalign != 0)
        {
            if (size % maxalign != 0)
            {
                size += maxalign - (size % maxalign);
            }
        }
        else
        {
            sizeof_error = ESIZEOF_INCOMPLETE;
            throw;
        }
    }
    catch
    {
        return sizeof_error;
    }

    *sz = size;
    return sizeof_error;
}

enum sizeof_error get_sizeof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier, size_t* sz, enum target target)
{
    enum sizeof_error sizeof_error = ESIZEOF_NONE;

    const bool is_union =
        (complete_struct_or_union_specifier->first_token->type == TK_KEYWORD_UNION);

    size_t size = 0;
    try
    {
        size_t maxalign = 0;

        struct member_declaration* _Opt d = complete_struct_or_union_specifier->member_declaration_list.head;
        while (d)
        {
            if (d->member_declarator_list_opt)
            {
                struct member_declarator* _Opt md = d->member_declarator_list_opt->head;
                while (md)
                {
                    size_t align = 1;

                    if (md->declarator)
                    {
                        align = type_get_alignof(&md->declarator->type, target);

                        if (align > maxalign)
                        {
                            maxalign = align;
                        }
                        if (size % align != 0)
                        {
                            size += align - (size % align);
                        }
                        size_t item_size = 0;
                        sizeof_error = type_get_sizeof(&md->declarator->type, &item_size, target);
                        if (sizeof_error != 0)
                            throw;

                        if (is_union)
                        {
                            if (item_size > size)
                                size = item_size;
                        }
                        else
                        {
                            size += item_size;
                        }
                    }
                    else
                    {
                        sizeof_error = ESIZEOF_INCOMPLETE;
                        throw;
                    }
                    md = md->next;
                }
            }
            else if (d->specifier_qualifier_list)
            {
                if (d->specifier_qualifier_list->struct_or_union_specifier)
                {
                    struct type t = { 0 };
                    t.category = TYPE_CATEGORY_ITSELF;
                    t.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;
                    t.type_specifier_flags = TYPE_SPECIFIER_STRUCT_OR_UNION;

                    size_t align = type_get_alignof(&t, target);

                    if (align > maxalign)
                    {
                        maxalign = align;
                    }
                    if (size % align != 0)
                    {
                        size += align - (size % align);
                    }
                    size_t item_size = 0;

                    sizeof_error = type_get_sizeof(&t, &item_size, target);
                    if (sizeof_error != 0)
                        throw;

                    if (is_union)
                    {
                        if (item_size > size)
                            size = item_size;
                    }
                    else
                    {
                        //TODO overflow
                        size += item_size;
                    }
                    type_destroy(&t);
                }
                else
                {
                    sizeof_error = ESIZEOF_INCOMPLETE;
                    throw;
                }
            }
            d = d->next;
        }
        if (maxalign != 0)
        {
            if (size % maxalign != 0)
            {
                size += maxalign - (size % maxalign);
            }
        }
        else
        {
            sizeof_error = ESIZEOF_INCOMPLETE;
            throw;
        }
    }
    catch
    {
        return sizeof_error;
    }

    *sz = size;
    return sizeof_error;
}

size_t type_get_alignof(const struct type* p_type, enum target target);
size_t get_alignof_struct(struct struct_or_union_specifier* complete_struct_or_union_specifier, enum target target)
{
    size_t align = 0;
    struct member_declaration* _Opt d = complete_struct_or_union_specifier->member_declaration_list.head;
    while (d)
    {
        if (d->member_declarator_list_opt)
        {
            struct member_declarator* _Opt md = d->member_declarator_list_opt->head;
            while (md)
            {
                if (md->declarator)
                {
                    size_t temp_align = type_get_alignof(&md->declarator->type, target);
                    if (temp_align > align)
                    {
                        align = temp_align;
                    }
                }
                else
                {
                    assert(false);
                }
                md = md->next;
            }
        }
        else if (d->specifier_qualifier_list)
        {
            /*We don't have the declarator like in */
            /*
              struct X {
                union {
                    struct {
                        int Zone;
                    };
                    int Value;
                };
            };
            static_assert(alignof(struct X) == 1);
            */

            /*so we create a type using only specifiers*/

            struct type type = { 0 };

            type.type_specifier_flags = d->specifier_qualifier_list->type_specifier_flags;

            type.enum_specifier = d->specifier_qualifier_list->enum_specifier;
            type.struct_or_union_specifier = d->specifier_qualifier_list->struct_or_union_specifier;


            size_t temp_align = type_get_alignof(&type, target);
            if (temp_align > align)
            {
                align = temp_align;
            }

            type_destroy(&type);
        }
        else
        {
            /*static_assert*/
        }
        d = d->next;
    }
    assert(align != 0);
    return align;
}

size_t type_get_alignof(const struct type* p_type, enum target target)
{
    size_t align = 0;

    enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        align = get_align_void_ptr(target);
    }
    else if (category == TYPE_CATEGORY_FUNCTION)
    {
        align = SIZE_MAX-1;
        //seterror(error, "sizeof function");
    }
    else if (category == TYPE_CATEGORY_ITSELF)
    {
        if (p_type->alignment_specifier_flags & ALIGNMENT_SPECIFIER_8_FLAGS)
        {
            align = 8;
        }
        else if (p_type->alignment_specifier_flags & ALIGNMENT_SPECIFIER_16_FLAGS)
        {
            align = 16;
        }
        else if (p_type->alignment_specifier_flags & ALIGNMENT_SPECIFIER_32_FLAGS)
        {
            align = 32;
        }
        else if (p_type->alignment_specifier_flags & ALIGNMENT_SPECIFIER_64_FLAGS)
        {
            align = 64;
        }
        else if (p_type->alignment_specifier_flags & ALIGNMENT_SPECIFIER_128_FLAGS)
        {
            align = 128;
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
        {
            align = get_align_char(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
        {
            align = get_align_bool(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
        {
            align = get_align_short(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
        {
            if (p_type->enum_specifier)
            {
                enum type_specifier_flags enum_type_specifier_flags =
                    get_enum_type_specifier_flags(p_type->enum_specifier);

                struct type t = make_with_type_specifier_flags(enum_type_specifier_flags);
                align = type_get_alignof(&t, target);
                type_destroy(&t);
            }
            else
                align = get_align_int(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
        {
            align = get_align_long(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
        {
            align = get_align_long_long(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT) //must be after long
        {
            align = get_align_int(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
        {
            align = get_align_float(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
        {
            align = get_align_double(target);
        }
        else if (p_type->type_specifier_flags & (TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE))
        {
            align = get_align_long_double(target);
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_GCC__BUILTIN_VA_LIST)
        {
#if __GNUC__
            align = _Alignof(__builtin_va_list);
#else
            align = get_align_void_ptr(target); //?            
#endif
        }
        else if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
        {
            if (p_type->struct_or_union_specifier)
            {
                struct struct_or_union_specifier* _Opt p_complete =
                    get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

                align = 1;
                if (p_complete)
                {
                    align = get_alignof_struct(p_complete, target);
                }
                else
                {
                    align = SIZE_MAX - 2;
                }
            }
            else
            {
                align = SIZE_MAX - 2;
                assert(false);
            }
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            align = SIZE_MAX - 3;
            //seterror(error, "type information is missing");
        }
        else if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
        {
            align = 1;
        }
        else
        {
            assert(false);
        }
    }
    else if (category == TYPE_CATEGORY_ARRAY)
    {

        struct type type = get_array_item_type(p_type);
        align = type_get_alignof(&type, target);
        type_destroy(&type);
    }
    assert(align > 0);
    return align;
}

enum sizeof_error type_get_offsetof(const struct type* p_type, const char* member, size_t* size, enum target target)
{
    *size = 0; //out

    const enum type_category category = type_get_category(p_type);

    if (category != TYPE_CATEGORY_ITSELF)
    {
        *size = sizeof(void*);
        return ESIZEOF_FUNCTION;
    }

    if (!(p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION))
    {
        return ESIZEOF_INCOMPLETE;
    }

    if (p_type->struct_or_union_specifier == NULL)
    {
        return ESIZEOF_INCOMPLETE;
    }

    struct struct_or_union_specifier* _Opt p_complete =
        get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

    if (p_complete == NULL)
        return ESIZEOF_INCOMPLETE;

    return get_offsetof_struct(p_complete, member, size, target);
}

enum sizeof_error type_get_sizeof(const struct type* p_type, size_t* size, enum target target)
{
    *size = 0; //out

    const enum type_category category = type_get_category(p_type);

    if (category == TYPE_CATEGORY_POINTER)
    {
        *size = get_size_void_ptr(target);
        return ESIZEOF_NONE;
    }

    if (category == TYPE_CATEGORY_FUNCTION)
    {
        return ESIZEOF_FUNCTION;
    }

    if (category == TYPE_CATEGORY_ARRAY)
    {
        if (p_type->storage_class_specifier_flags & STORAGE_SPECIFIER_PARAMETER)
        {
            //void f(int a[2])
            *size = get_size_void_ptr(target);
            return ESIZEOF_NONE;
        }
        else
        {
            if (type_is_vla(p_type))
                return ESIZEOF_VLA;

            unsigned long long arraysize = p_type->num_of_elements;
            struct type type = get_array_item_type(p_type);


            size_t sz = 0;

            const enum sizeof_error er = type_get_sizeof(&type, &sz, target);
            if (er != ESIZEOF_NONE)
            {
                type_destroy(&type);
                return er;
            }
            type_destroy(&type);


            unsigned long long result = 0;
            if (unsigned_long_long_mul(&result, sz, arraysize))
            {
                //https://github.com/thradams/cake/issues/248
                unsigned long long SIZE_MAX_WORKAROUND = 0;

#ifdef __linux__
#if __x86_64__
                SIZE_MAX_WORKAROUND = 0xffffffffffffffffULL;
#else
                SIZE_MAX_WORKAROUND = 0xffffffffULL;
#endif                    
#else                
                SIZE_MAX_WORKAROUND = SIZE_MAX;
#endif

                if (result > SIZE_MAX_WORKAROUND)
                {
                    return ESIZEOF_OVERLOW;
                }

                //
                if (result > /*SIZEMAX*/ 4294967295)
                {
                    return ESIZEOF_OVERLOW;
                }
                *size = (size_t)result;
            }
            else
            {
                return ESIZEOF_OVERLOW;
            }
            return ESIZEOF_NONE;
        }
    }

    assert(category == TYPE_CATEGORY_ITSELF);


    if (p_type->type_specifier_flags & TYPE_SPECIFIER_CHAR)
    {
        *size = get_size_char(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_BOOL)
    {
        *size = get_size_bool(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_SHORT)
    {
        *size = get_size_short(target);
        return ESIZEOF_NONE;
    }

    else if (p_type->type_specifier_flags & TYPE_SPECIFIER_GCC__BUILTIN_VA_LIST)
    {
#if __GNUC__
        * size = sizeof(__builtin_va_list);
#else
        * size = get_size_void_ptr(target); //?            
#endif
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG)
    {
        *size = get_size_long(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_LONG_LONG)
    {
        *size = get_size_long_long(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_INT) //must be after long
    {
        //typedef long unsigned int uint64_t;
        *size = get_size_int(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_FLOAT)
    {
        *size = get_size_float(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_DOUBLE)
    {
        *size = get_size_double(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & (TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE))
    {
        *size = get_size_long_double(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_STRUCT_OR_UNION)
    {
        if (p_type->struct_or_union_specifier == NULL)
        {
            return ESIZEOF_INCOMPLETE;
        }

        struct struct_or_union_specifier* _Opt p_complete =
            get_complete_struct_or_union_specifier(p_type->struct_or_union_specifier);

        if (p_complete == NULL)
            return ESIZEOF_INCOMPLETE;

        return get_sizeof_struct(p_complete, size, target);
    }

    if (p_type->type_specifier_flags & TYPE_SPECIFIER_ENUM)
    {
        if (p_type->enum_specifier)
        {
            enum type_specifier_flags enum_type_specifier_flags =
                get_enum_type_specifier_flags(p_type->enum_specifier);

            struct type t = make_with_type_specifier_flags(enum_type_specifier_flags);
            enum sizeof_error e = type_get_sizeof(&t, size, target);
            type_destroy(&t);
            return e;
        }
        else
        {
            *size = get_size_int(target);
        }
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags == TYPE_SPECIFIER_NONE)
    {
        *size = 0;
        return ESIZEOF_INCOMPLETE;
    }

    if (p_type->type_specifier_flags == TYPE_SPECIFIER_VOID)
    {
        *size = 1;
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags == TYPE_SPECIFIER_NULLPTR_T)
    {
        *size = get_size_void_ptr(target);
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags == TYPE_SPECIFIER_DECIMAL32)
    {
        *size = 4;
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags == TYPE_SPECIFIER_DECIMAL64)
    {
        *size = 8;
        return ESIZEOF_NONE;
    }

    if (p_type->type_specifier_flags == TYPE_SPECIFIER_DECIMAL128)
    {
        *size = 16;
        return ESIZEOF_NONE;
    }


    return ESIZEOF_INCOMPLETE;
}

void type_set_attributes(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->attributes_flags =
            pdeclarator->declaration_specifiers->attributes_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        //p_type->type_qualifier_flags =
          //  pdeclarator->specifier_qualifier_list->ATR;
    }
}

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator);

bool function_returns_void(const struct type* p_type)
{
    struct type t = get_function_return_type(p_type);
    bool r = type_is_void(&t);
    type_destroy(&t);
    return r;
}

struct type get_function_return_type(const struct type* p_type)
{
    try
    {
        if (p_type->next == NULL)
            throw;

        if (type_is_pointer(p_type))
        {
            if (p_type->next->next == NULL)
            {
                throw;
            }

            /*pointer to function returning ... */
            struct type r = type_dup(p_type->next->next);
            return r;
        }

        /*function returning ... */
        struct type r = type_dup(p_type->next);
        return r;
    }
    catch
    {
    }
    struct type empty = { 0 };
    return empty;
}


void type_set_int(struct type* p_type)
{
    p_type->type_specifier_flags = TYPE_SPECIFIER_INT;
    p_type->type_qualifier_flags = 0;
    p_type->category = TYPE_CATEGORY_ITSELF;
}

struct type type_make_enumerator(const struct enum_specifier* enum_specifier)
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_ENUM;
    t.enum_specifier = enum_specifier;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_get_enum_type(const struct type* p_type)
{
    try
    {
        if (p_type->enum_specifier == NULL)
            throw;

        const struct enum_specifier* _Opt p_complete_enum_specifier =
            get_complete_enum_specifier(p_type->enum_specifier);

        if (p_complete_enum_specifier &&
            p_complete_enum_specifier->specifier_qualifier_list)
        {
            struct type t = { 0 };
            t.type_qualifier_flags = p_complete_enum_specifier->specifier_qualifier_list->type_qualifier_flags;
            t.type_specifier_flags = p_complete_enum_specifier->specifier_qualifier_list->type_specifier_flags;
            return t;
        }

        struct type t = { 0 };
        t.type_specifier_flags = TYPE_SPECIFIER_INT;
        return t;
    }
    catch
    {
    }
    struct type empty = { 0 };
    return empty;
}

struct type type_make_long_double()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_LONG | TYPE_SPECIFIER_DOUBLE;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_double()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_DOUBLE;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_float()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_FLOAT;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_ptrdiff_t(enum target target)
{
    struct type t = { 0 };
    t.type_specifier_flags = get_ptrdiff_t_specifier(target);
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_size_t(enum target target)
{
    struct type t = { 0 };
    t.type_specifier_flags = get_size_t_specifier(target);
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type make_void_ptr_type()
{
    struct type t = { 0 };
    try
    {
        struct type* _Owner _Opt p = calloc(1, sizeof * p);
        if (p == NULL) throw;

        t.category = TYPE_CATEGORY_POINTER;
        p->category = TYPE_CATEGORY_ITSELF;
        p->type_specifier_flags = TYPE_SPECIFIER_VOID;
        t.next = p;
    }
    catch
    {

    }
    return t;
}

struct type make_void_type()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_VOID;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_int_bool_like()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    t.attributes_flags = CAKE_HIDDEN_ATTRIBUTE_LIKE_BOOL;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type make_with_type_specifier_flags(enum type_specifier_flags f)
{
    struct type t = { 0 };
    t.type_specifier_flags = f;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type make_size_t_type(enum target target)
{
    struct type t = { 0 };
    t.type_specifier_flags = get_size_t_specifier(target);

    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_int()
{
    struct type t = { 0 };
    t.type_specifier_flags = TYPE_SPECIFIER_INT;
    t.category = TYPE_CATEGORY_ITSELF;
    return t;
}

struct type type_make_literal_string(int size_in_bytes,
    enum type_specifier_flags chartype,
    enum type_qualifier_flags qualifiers,
    enum target target)
{
    struct type t = { 0 };

    try
    {
        struct type* _Owner _Opt p2 = calloc(1, sizeof(struct type));
        if (p2 == NULL) throw;

        struct type char_type = { 0 };
        char_type.category = TYPE_CATEGORY_ITSELF;
        char_type.type_specifier_flags = chartype;

        size_t char_size = 0;

        if (type_get_sizeof(&char_type, &char_size, target) != 0)
        {
            type_delete(p2);
            throw;
        }

        if (char_size == 0)
        {
            char_size = 1;
        }
        type_destroy(&char_type);


        t.category = TYPE_CATEGORY_ARRAY;
        t.num_of_elements = size_in_bytes / char_size;

        p2->category = TYPE_CATEGORY_ITSELF;
        p2->type_specifier_flags = chartype;
        p2->type_qualifier_flags = qualifiers;
        t.next = p2;
    }
    catch
    {
    }

    return t;
}

bool struct_or_union_specifier_is_same(struct struct_or_union_specifier* _Opt a, struct struct_or_union_specifier* _Opt b)
{
    if (a && b)
    {
        struct struct_or_union_specifier* _Opt p_complete_a = get_complete_struct_or_union_specifier(a);
        struct struct_or_union_specifier* _Opt p_complete_b = get_complete_struct_or_union_specifier(b);

        if (p_complete_a != NULL && p_complete_b != NULL)
        {
            if (p_complete_a != p_complete_b)
            {
                return false;
            }
            return true;
        }
        else
        {
            /*both incomplete then we compare tag names*/
            if (a->tagtoken != NULL && b->tagtoken != NULL)
            {
                if (strcmp(a->tagtoken->lexeme, b->tagtoken->lexeme) == 0)
                    return true;
            }
        }
        return p_complete_a == NULL && p_complete_b == NULL;
    }
    return a == NULL && b == NULL;
}

bool enum_specifier_is_same(struct enum_specifier* _Opt a, struct enum_specifier* _Opt b)
{
    if (a && b)
    {
        if (get_complete_enum_specifier(a) && get_complete_enum_specifier(b))
        {
            if (get_complete_enum_specifier(a) != get_complete_enum_specifier(b))
            {
                return false;
            }
            return true;
        }
        return get_complete_enum_specifier(a) == NULL &&
            get_complete_enum_specifier(b) == NULL;
    }
    return a == NULL && b == NULL;
}


static bool type_is_same_core(const struct type* a,
                              const struct type* b,
                              bool compare_qualifiers)
{
    const struct type* _Opt pa = a;
    const struct type* _Opt pb = b;

    while (pa && pb)
    {
        if (pa->num_of_elements != pb->num_of_elements)
            return false;

        if (pa->category != pb->category)
            return false;

        if (pa->enum_specifier &&
            pb->enum_specifier &&
            get_complete_enum_specifier(pa->enum_specifier) !=
            get_complete_enum_specifier(pb->enum_specifier))
        {
            return false;
        }


        if (pa->enum_specifier && !pb->enum_specifier)
        {
            //TODO enum with types
            //enum  x int
           //return false;
        }

        if (!pa->enum_specifier && pb->enum_specifier)
        {
            //TODO enum with types
            //int x enum
            //return false;
        }

        //if (pa->name_opt != pb->name_opt) return false;
        if (pa->has_static_array_size != pb->has_static_array_size)
            return false;

        if (pa->category == TYPE_CATEGORY_FUNCTION)
        {

            if (pa->params.is_var_args != pb->params.is_var_args)
            {
                return false;
            }

            if (pa->params.is_void != pb->params.is_void)
            {
                return false;
            }

            struct param* _Opt p_param_a = pa->params.head;
            struct param* _Opt p_param_b = pb->params.head;
            while (p_param_a && p_param_b)
            {
                if (!type_is_same(&p_param_a->type, &p_param_b->type, compare_qualifiers))
                {
                    return false;
                }
                p_param_a = p_param_a->next;
                p_param_b = p_param_b->next;
            }
            return p_param_a == NULL && p_param_b == NULL;
        }

        if (pa->struct_or_union_specifier &&
            pb->struct_or_union_specifier)
        {

            if (pa->struct_or_union_specifier->complete_struct_or_union_specifier_indirection !=
                pb->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
            {
                //this should work but it is not...
            }

            if (strcmp(pa->struct_or_union_specifier->tag_name, pb->struct_or_union_specifier->tag_name) != 0)
            {
                return false;
            }
        }

        if (compare_qualifiers)
        {
            enum type_qualifier_flags aq = pa->type_qualifier_flags;
            enum type_qualifier_flags bq = pb->type_qualifier_flags;

            unsigned int all = (TYPE_QUALIFIER_OWNER | TYPE_QUALIFIER_VIEW |
             TYPE_QUALIFIER_OPT | TYPE_QUALIFIER_DTOR | TYPE_QUALIFIER_CTOR);

            aq = aq & ~all;
            bq = bq & ~all;

            if (aq != bq)
                return false;
        }


        enum type_specifier_flags a_flags = pa->type_specifier_flags;
        enum type_specifier_flags b_flags = pb->type_specifier_flags;

        if ((a_flags & TYPE_SPECIFIER_CHAR) == 0)
        {
            a_flags &= ~TYPE_SPECIFIER_SIGNED;
        }

        if ((b_flags & TYPE_SPECIFIER_CHAR) == 0)
        {
            b_flags &= ~TYPE_SPECIFIER_SIGNED;
        }

        if (a_flags != b_flags)
        {
            return false;
        }


        pa = pa->next;
        pb = pb->next;
    }
    return pa == NULL && pb == NULL;
}

bool type_is_same(const struct type* a, const struct type* b, bool compare_qualifiers)
{
    return type_is_same_core(a, b, compare_qualifiers);
}

bool type_is_compatible(const struct type* a, const struct type* b)
{
    return type_is_same_core(a, b, false);
}

void type_clear(struct type* a)
{
    struct type tmp = { 0 };
    type_swap(&tmp, a);
    type_destroy(&tmp);
}

void type_swap(_View struct type* a, _View struct type* b)
{
    _View struct type temp = *a;
    *a = *b;
    *b = temp;
}


void type_visit_to_mark_anonymous(struct type* p_type)
{
    //TODO better visit?
    if (p_type->struct_or_union_specifier != NULL &&
        p_type->struct_or_union_specifier->has_anonymous_tag)
    {
        if (p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection)
        {
            p_type->struct_or_union_specifier->complete_struct_or_union_specifier_indirection->show_anonymous_tag = true;
        }
        p_type->struct_or_union_specifier->show_anonymous_tag = true;
    }

}


void type_merge_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{

    enum type_qualifier_flags type_qualifier_flags = 0;


    if (pdeclarator->declaration_specifiers)
    {
        type_qualifier_flags = pdeclarator->declaration_specifiers->type_qualifier_flags;


    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        type_qualifier_flags = pdeclarator->specifier_qualifier_list->type_qualifier_flags;


    }

    p_type->type_qualifier_flags |= type_qualifier_flags;





}


void type_set_qualifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{

    enum type_qualifier_flags type_qualifier_flags = 0;
    if (pdeclarator->declaration_specifiers)
    {
        type_qualifier_flags = pdeclarator->declaration_specifiers->type_qualifier_flags;

    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        type_qualifier_flags = pdeclarator->specifier_qualifier_list->type_qualifier_flags;

    }

    p_type->type_qualifier_flags = type_qualifier_flags;


}
void type_set_alignment_specifier_flags_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->alignment_specifier_flags |=
            pdeclarator->declaration_specifiers->alignment_specifier_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        p_type->alignment_specifier_flags =
            pdeclarator->specifier_qualifier_list->alignment_specifier_flags;
    }
}

void type_set_msvc_declspec_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->msvc_declspec_flags |=
            pdeclarator->declaration_specifiers->msvc_declspec_flags;
    }
}

void type_set_storage_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->storage_class_specifier_flags |=
            pdeclarator->declaration_specifiers->storage_class_specifier_flags;
    }
    else
    {
        //struct member
        //assert(false);
        /*
           where we don't have specifiers?
        */
        //p_type->storage_class_specifier_flags |= STORAGE_SPECIFIER_AUTO;
    }
}


void type_set_specifiers_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        p_type->type_specifier_flags =
            pdeclarator->declaration_specifiers->type_specifier_flags;

        p_type->enum_specifier = pdeclarator->declaration_specifiers->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->declaration_specifiers->struct_or_union_specifier;

    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        p_type->type_specifier_flags =
            pdeclarator->specifier_qualifier_list->type_specifier_flags;
        p_type->enum_specifier = pdeclarator->specifier_qualifier_list->enum_specifier;
        p_type->struct_or_union_specifier = pdeclarator->specifier_qualifier_list->struct_or_union_specifier;

    }


}

void type_set_attributes_using_declarator(struct type* p_type, struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        if (pdeclarator->declaration_specifiers->attributes_flags & STD_ATTRIBUTE_NODISCARD)
        {
            p_type->storage_class_specifier_flags |= STORAGE_SPECIFIER_FUNCTION_RETURN_NODISCARD;
        }
        p_type->attributes_flags =
            pdeclarator->declaration_specifiers->attributes_flags;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        //p_type->attributes_flags =
          //  pdeclarator->specifier_qualifier_list->attributes_flags;
    }
}


void type_list_push_front(struct type_list* books, struct type* _Owner new_book)
{
    assert(new_book->next == NULL);

    if (books->head == NULL)
    {
        books->head = new_book;
        books->tail = new_book;
    }
    else
    {
        new_book->next = books->head;
        books->head = new_book;
    }
}

void type_list_destroy(_Dtor struct type_list* p_type_list)
{
    struct type* _Owner _Opt item = p_type_list->head;
    while (item)
    {
        struct type* _Owner _Opt next = item->next;
        item->next = NULL;
        type_destroy_one(item);
        free(item);
        item = next;
    }
}

void type_list_push_back(struct type_list* type_list, struct type* _Owner new_book)
{
    if (type_list->tail == NULL)
    {
        assert(type_list->head == NULL);
        type_list->head = new_book;
    }
    else
    {
        assert(type_list->tail->next == NULL);
        type_list->tail->next = new_book;
    }

    type_list->tail = new_book;
}

void make_type_using_declarator_core(struct parser_ctx* ctx, struct declarator* pdeclarator, char** ppname, struct type_list* list);

void  make_type_using_direct_declarator(struct parser_ctx* ctx,
    struct direct_declarator* pdirectdeclarator,
    char** ppname,
    struct type_list* list)
{
    try
    {
        if (pdirectdeclarator->declarator)
        {
            make_type_using_declarator_core(ctx, pdirectdeclarator->declarator, ppname, list);
        }

        else if (pdirectdeclarator->function_declarator)
        {
            if (pdirectdeclarator->function_declarator->direct_declarator)
            {
                make_type_using_direct_declarator(ctx,
                    pdirectdeclarator->function_declarator->direct_declarator,
                    ppname,
                    list);
            }

            struct type* _Owner _Opt p_func = calloc(1, sizeof(struct type));
            if (p_func == NULL) throw;

            p_func->category = TYPE_CATEGORY_FUNCTION;

            assert(pdirectdeclarator->function_declarator->direct_declarator != NULL);
            if (pdirectdeclarator->function_declarator->direct_declarator->p_calling_convention)
            {
                const char* calling_convention_lexeme =
                    pdirectdeclarator->function_declarator->direct_declarator->p_calling_convention->lexeme;

                if (strcmp(calling_convention_lexeme, "__fastcall") == 0)
                    p_func->attributes_flags |= CAKE_ATTRIBUTE_FASTCALL;
                else if (strcmp(calling_convention_lexeme, "__stdcall") == 0)
                    p_func->attributes_flags |= CAKE_ATTRIBUTE_STDCALL;
                else if (strcmp(calling_convention_lexeme, "__cdecl") == 0)
                    p_func->attributes_flags |= CAKE_ATTRIBUTE_CDECL;
                else
                {
                    type_delete(p_func);
                    throw;
                }
            }

            if (pdirectdeclarator->function_declarator->parameter_type_list_opt)
            {
                p_func->params.is_var_args = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_var_args;
                p_func->params.is_void = pdirectdeclarator->function_declarator->parameter_type_list_opt->is_void;
            }

            if (pdirectdeclarator->function_declarator->parameter_type_list_opt &&
                pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list)
            {

                struct parameter_declaration* _Opt p =
                    pdirectdeclarator->function_declarator->parameter_type_list_opt->parameter_list->head;

                while (p)
                {
                    if (p->declarator == NULL)
                    {
                        type_delete(p_func);
                        throw;
                    }

                    struct param* _Owner _Opt p_new_param = calloc(1, sizeof(struct param));
                    if (p_new_param == NULL)
                    {
                        type_delete(p_func);
                        throw;
                    }

                    p_new_param->type = type_dup(&p->declarator->type);
                    param_list_add(&p_func->params, p_new_param);
                    p = p->next;
                }
            }


            type_list_push_back(list, p_func);
        }
        else if (pdirectdeclarator->array_declarator)
        {

            if (pdirectdeclarator->array_declarator->direct_declarator)
            {
                make_type_using_direct_declarator(ctx,
                    pdirectdeclarator->array_declarator->direct_declarator,
                    ppname,
                    list);
            }

            struct type* _Owner _Opt  p = calloc(1, sizeof(struct type));
            if (p == NULL) throw;

            p->category = TYPE_CATEGORY_ARRAY;

            
            p->num_of_elements =
                array_declarator_get_size(pdirectdeclarator->array_declarator);

            p->array_num_elements_expression = pdirectdeclarator->array_declarator->assignment_expression;

            if (pdirectdeclarator->array_declarator->static_token_opt)
            {
                p->has_static_array_size = true;
            }

            if (pdirectdeclarator->array_declarator->type_qualifier_list_opt)
            {
                p->type_qualifier_flags = pdirectdeclarator->array_declarator->type_qualifier_list_opt->flags;
            }

            type_list_push_back(list, p);

            // if (pdirectdeclarator->name_opt)
             //{
               //  p->name_opt = strdup(pdirectdeclarator->name_opt->lexeme);
             //}
        }

        if (pdirectdeclarator->name_opt)
        {
            *ppname = pdirectdeclarator->name_opt->lexeme;
        }

    }
    catch
    {
        //tODO
    }
}

void make_type_using_declarator_core(struct parser_ctx* ctx, struct declarator* pdeclarator,
    char** ppname, struct type_list* list)
{
    try
    {
        struct type_list pointers = { 0 };
        struct pointer* _Opt pointer = pdeclarator->pointer;
        while (pointer)
        {
            struct type* _Owner _Opt p_flat = calloc(1, sizeof(struct type));
            if (p_flat == NULL)
            {
                type_list_destroy(&pointers);
                throw;
            }

            if (pointer->type_qualifier_list_opt)
            {
                p_flat->type_qualifier_flags = pointer->type_qualifier_list_opt->flags;
            }

            if (pointer->attribute_specifier_sequence_opt)
            {
                p_flat->attributes_flags |= pointer->attribute_specifier_sequence_opt->attributes_flags;
            }
            p_flat->category = TYPE_CATEGORY_POINTER;

            if (pointer->calling_convention)
            {
                const char* calling_convention_lexeme =
                    pointer->calling_convention->lexeme;
                if (strcmp(calling_convention_lexeme, "__fastcall") == 0)
                    p_flat->attributes_flags |= CAKE_ATTRIBUTE_FASTCALL;
                else if (strcmp(calling_convention_lexeme, "__stdcall") == 0)
                    p_flat->attributes_flags |= CAKE_ATTRIBUTE_STDCALL;
                else if (strcmp(calling_convention_lexeme, "__cdecl") == 0)
                    p_flat->attributes_flags |= CAKE_ATTRIBUTE_CDECL;
                else
                {
                    type_list_destroy(&pointers);
                    type_delete(p_flat);
                    throw;
                }
            }

            type_list_push_front(&pointers, p_flat); /*inverted*/
            pointer = pointer->pointer;
        }

        if (pdeclarator->direct_declarator)
        {
            make_type_using_direct_declarator(ctx, pdeclarator->direct_declarator, ppname, list);
            if (list->head &&
                list->head->category == TYPE_CATEGORY_FUNCTION)
            {
                if (pointers.head)
                {
                    pointers.head->storage_class_specifier_flags |= STORAGE_SPECIFIER_FUNCTION_RETURN;
                }
            }
        }

        while (pointers.head)
        {
            struct type* _Owner p = pointers.head;
            pointers.head = p->next;
            p->next = NULL;
            type_list_push_back(list, p);
        }
    }
    catch
    {
    }
}

struct enum_specifier* _Opt declarator_get_enum_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers &&
        pdeclarator->declaration_specifiers->enum_specifier)
    {
        return pdeclarator->declaration_specifiers->enum_specifier;
    }
    if (pdeclarator->specifier_qualifier_list &&
        pdeclarator->specifier_qualifier_list->enum_specifier)
    {
        return pdeclarator->specifier_qualifier_list->enum_specifier;
    }
    return NULL;
}


struct struct_or_union_specifier* _Opt declarator_get_struct_or_union_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers &&
        pdeclarator->declaration_specifiers->struct_or_union_specifier)
    {
        return pdeclarator->declaration_specifiers->struct_or_union_specifier;
    }
    if (pdeclarator->specifier_qualifier_list &&
        pdeclarator->specifier_qualifier_list->struct_or_union_specifier)
    {
        return pdeclarator->specifier_qualifier_list->struct_or_union_specifier;
    }
    return NULL;
}

struct typeof_specifier* _Opt declarator_get_typeof_specifier(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typeof_specifier;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        return pdeclarator->specifier_qualifier_list->typeof_specifier;
    }
    return NULL;
}

struct declarator* _Opt declarator_get_typedef_declarator(struct declarator* pdeclarator)
{
    if (pdeclarator->declaration_specifiers)
    {
        return pdeclarator->declaration_specifiers->typedef_declarator;
    }
    else if (pdeclarator->specifier_qualifier_list)
    {
        return pdeclarator->specifier_qualifier_list->typedef_declarator;
    }

    return NULL;
}

static bool is_valid_type(struct parser_ctx* ctx, struct token* _Opt p_token, const struct type* p_type)
{
    if (p_token == NULL)
        p_token = ctx->current;

    const struct type* _Opt p = p_type;
    while (p)
    {
        if (p->category == TYPE_CATEGORY_FUNCTION)
        {
            if (p->next && p->next->category == TYPE_CATEGORY_FUNCTION)
            {
                compiler_diagnostic(C_ERROR_FUNCTION_RETURNS_FUNCTION,
                                            ctx,
                                            p_token,
                                            NULL,
                                            "function returning function");
                return false;
            }
            else if (p->next && p->next->category == TYPE_CATEGORY_ARRAY)
            {
                compiler_diagnostic(C_ERROR_FUNCTION_RETURNS_ARRAY,
                                            ctx,
                                            p_token,
                                            NULL,
                                            "function returning array");
                return false;
            }
        }
        else if (p->category == TYPE_CATEGORY_ITSELF &&
                 p->type_specifier_flags == TYPE_SPECIFIER_NONE)
        {
            compiler_diagnostic(C_ERROR_INVALID_TYPE,
                                        ctx,
                                        p_token,
                                        NULL,
                                        "invalid type");
            return false;
        }

        p = p->next;
    }

    return true;
}

struct type make_type_using_declarator(struct parser_ctx* ctx, struct declarator* pdeclarator)
{

    try
    {
        struct type_list list = { 0 };

        char* _Opt name = NULL;
        make_type_using_declarator_core(ctx, pdeclarator, &name, &list);


        if (declarator_get_typeof_specifier(pdeclarator))
        {
            struct type* _Owner _Opt p_nt = calloc(1, sizeof(struct type));
            if (p_nt == NULL)
            {
                type_list_destroy(&list);
                throw;
            }

            struct type nt =
                type_dup(&declarator_get_typeof_specifier(pdeclarator)->type);

            *p_nt = nt;


            if (list.head != NULL)
                type_set_qualifiers_using_declarator(list.head, pdeclarator);

            if (list.tail)
            {
                assert(list.tail->next == NULL);
                list.tail->next = p_nt;
            }
            else
            {
                type_list_push_back(&list, p_nt);
            }
        }
        else if (declarator_get_typedef_declarator(pdeclarator))
        {
            struct declarator* _Opt p_typedef_declarator =
                declarator_get_typedef_declarator(pdeclarator);

            if (p_typedef_declarator == NULL)
            {
                type_list_destroy(&list);
                throw;
            }

            struct type nt =
                type_dup(&p_typedef_declarator->type);

            struct type* _Owner _Opt p_nt = calloc(1, sizeof(struct type));
            if (p_nt == NULL)
            {
                type_list_destroy(&list);
                type_destroy(&nt);
                throw;
            }

            *p_nt = nt;


            /*
              maybe typedef already has const qualifier
              so we cannot override
            */
            type_merge_qualifiers_using_declarator(p_nt, pdeclarator);

            if (list.tail)
            {
                assert(list.tail->next == 0);
                list.tail->next = p_nt;
            }
            else
            {
                type_list_push_back(&list, p_nt);
            }
        }
        else
        {
            struct type* _Owner _Opt p = calloc(1, sizeof(struct type));
            if (p == NULL)
            {
                type_list_destroy(&list);
                throw;
            }

            p->category = TYPE_CATEGORY_ITSELF;


            type_set_specifiers_using_declarator(p, pdeclarator);
            type_set_attributes_using_declarator(p, pdeclarator);
            type_set_alignment_specifier_flags_using_declarator(p, pdeclarator);

            type_set_qualifiers_using_declarator(p, pdeclarator);

            if (list.tail &&
                list.tail->category == TYPE_CATEGORY_FUNCTION)
            {
                p->storage_class_specifier_flags |= STORAGE_SPECIFIER_FUNCTION_RETURN;
            }

            type_list_push_back(&list, p);

            if (list.head)
                type_set_storage_specifiers_using_declarator(list.head, pdeclarator);
        }


        if (list.head == NULL) throw;

        if (pdeclarator->name_opt)
        {
            char* _Owner _Opt temp = strdup(pdeclarator->name_opt->lexeme);
            if (temp == NULL)
            {
                type_list_destroy(&list);
                throw;
            }

            free((void* _Owner) list.head->name_opt);
            list.head->name_opt = temp;
        }

        struct type r = *list.head;
        /*
          we moved the contents of head
          but we also need to delete the memory
        */
        free(list.head);

        type_set_storage_specifiers_using_declarator(&r, pdeclarator);
        type_set_msvc_declspec_using_declarator(&r, pdeclarator);
        type_set_alignment_specifier_flags_using_declarator(&r, pdeclarator);
        if (!is_valid_type(ctx, pdeclarator->first_token_opt, &r))
        {
            type_destroy(&r);
            struct type empty = { 0 };
            return empty;
        }
        return r;
    }
    catch
    {

    }

    struct type empty = { 0 };
    return empty;
}

void type_remove_names(struct type* p_type)
{
    /*
      function parameters names are preserved
    */
    struct type* _Opt p = p_type;

    while (p)
    {
        if (p->name_opt)
        {
            free((void* _Owner _Opt)p->name_opt);
            p->name_opt = NULL;
        }
        p = p->next;
    }
}

const struct type* type_get_specifer_part(const struct type* p_type)
{
    /*
     last part is the specifier
    */
    const struct type* _Opt p = p_type;
    while (p->next) p = p->next;
    return p;
}

