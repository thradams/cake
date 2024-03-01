//#include <stdint.h>
//#include <stdio.h>
typedef unsigned char uint8_t;
void printf(...);
void puts(const char* s);
void putchar(char c);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
typedef union {
	struct {
	} b0;
	struct {
	} b1;
	struct {
	} b2;
	struct {
	} b3;
	struct {
	} b4;
	struct {
	} b5;
	struct {
	} b6;
	struct {
	} b7;
} null_type;
typedef union {
	struct {
	} b0;
	struct {
	} b1;
	struct {
	} b2;
	struct {
	} b3;
	struct {
	} b4;
	struct {
	} b5;
	struct {
	} b6;
	struct {
	} b7;
} full_type;
typedef union {
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(0)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_FALSE;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(1)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_TRUE;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(2)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_NULL;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(3)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_NUMBER;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(4)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_STRING;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(5)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_ARRAY;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(6)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_OBJECT;
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)(-1)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_INVALID;
} json_type;
typedef union {
	union {
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic((char(*)[2
			- !((uint8_t)((uint8_t)0)
				& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_INVALID;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_FALSE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_FALSE;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_TRUE;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)(((uint8_t)1
					<< (_Generic((json_type
						*)
		{
			0
		}
		-> JSON_TRUE.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type
							*)
		{
			0
		}
		-> JSON_TRUE.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type
				*)
		{
			0
		}
		-> JSON_TRUE.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7))))
					| ((uint8_t)1
						<< (_Generic((json_type
							*)
		{
			0
		}
		-> JSON_FALSE
			.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
							| _Generic(
								(json_type*)
		{
			0
		}
		-> JSON_FALSE.b1,
			typeof((*(null_type
				*)0)
				.b1) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b1) : ((uint8_t)1
					<< 1))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b2,
			typeof((*(null_type
				*)0)
				.b2) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b2) : ((uint8_t)1
					<< 2))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b3,
			typeof((*(null_type
				*)0)
				.b3) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b3) : ((uint8_t)1
					<< 3))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b4,
			typeof((*(null_type
				*)0)
				.b4) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b4) : ((uint8_t)1
					<< 4))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b5,
			typeof((*(null_type
				*)0)
				.b5) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b5) : ((uint8_t)1
					<< 5))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b6,
			typeof((*(null_type
				*)0)
				.b6) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b6) : ((uint8_t)1
					<< 6))
			| _Generic(
				(json_type*)
		{
			0
		}
		-> JSON_FALSE.b7,
			typeof((*(null_type
				*)0)
				.b7) : (uint8_t)0,
			typeof((*(full_type
				*)0)
				.b7) : ((uint8_t)1
					<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_BOOLEAN;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NULL.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NULL.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NULL.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_NULL;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_NUMBER.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_NUMBER.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_NUMBER;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_STRING.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_STRING.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_STRING.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_STRING;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_ARRAY.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_ARRAY.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_ARRAY;
	union {
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 0))])0,
			char(*)[1]: (*(null_type*)0).b0,
			char(*)[2] : (*(full_type*)0).b0)) b0;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 1))])0,
			char(*)[1]: (*(null_type*)0).b1,
			char(*)[2] : (*(full_type*)0).b1)) b1;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 2))])0,
			char(*)[1]: (*(null_type*)0).b2,
			char(*)[2] : (*(full_type*)0).b2)) b2;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 3))])0,
			char(*)[1]: (*(null_type*)0).b3,
			char(*)[2] : (*(full_type*)0).b3)) b3;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 4))])0,
			char(*)[1]: (*(null_type*)0).b4,
			char(*)[2] : (*(full_type*)0).b4)) b4;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 5))])0,
			char(*)[1]: (*(null_type*)0).b5,
			char(*)[2] : (*(full_type*)0).b5)) b5;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 6))])0,
			char(*)[1]: (*(null_type*)0).b6,
			char(*)[2] : (*(full_type*)0).b6)) b6;
		typeof(_Generic(
			(char(*)[2
				- !((uint8_t)((
					(uint8_t)1
					<< (_Generic((json_type*) { 0 }
		-> JSON_OBJECT.b0,
			typeof((*(null_type*)0).b0) : (
				uint8_t)0,
			typeof((*(full_type*)0).b0) : (
				(uint8_t)1
				<< 0))
						| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b1,
			typeof((*(null_type*)0).b1) : (
				uint8_t)0,
			typeof((*(full_type*)0).b1) : (
				(uint8_t)1
				<< 1))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b2,
			typeof((*(null_type*)0).b2) : (
				uint8_t)0,
			typeof((*(full_type*)0).b2) : (
				(uint8_t)1
				<< 2))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b3,
			typeof((*(null_type*)0).b3) : (
				uint8_t)0,
			typeof((*(full_type*)0).b3) : (
				(uint8_t)1
				<< 3))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b4,
			typeof((*(null_type*)0).b4) : (
				uint8_t)0,
			typeof((*(full_type*)0).b4) : (
				(uint8_t)1
				<< 4))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b5,
			typeof((*(null_type*)0).b5) : (
				uint8_t)0,
			typeof((*(full_type*)0).b5) : (
				(uint8_t)1
				<< 5))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b6,
			typeof((*(null_type*)0).b6) : (
				uint8_t)0,
			typeof((*(full_type*)0).b6) : (
				(uint8_t)1
				<< 6))
			| _Generic((json_type*) { 0 }
		-> JSON_OBJECT.b7,
			typeof((*(null_type*)0).b7) : (
				uint8_t)0,
			typeof((*(full_type*)0).b7) : (
				(uint8_t)1
				<< 7)))))
					& ((uint8_t)1 << 7))])0,
			char(*)[1]: (*(null_type*)0).b7,
			char(*)[2] : (*(full_type*)0).b7)) b7;
	} JSON_OBJECT;
} json_flag;
int
main(void)
{
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_FALSE",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_FALSE);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_TRUE",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_TRUE);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_NULL",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_NULL);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_NUMBER",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_NUMBER);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_STRING",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_STRING);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_ARRAY",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_ARRAY);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_OBJECT",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_OBJECT);
	printf("json_type::%-13s  %"
		"u"
		"\n",
		"JSON_INVALID",
		(struct {
		uint8_t JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT,
			JSON_INVALID;
	})
	{
		(_Generic((json_type*) { 0 } -> JSON_FALSE.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_type*) { 0 } -> JSON_FALSE.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_type*) { 0 } -> JSON_INVALID.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_type*) { 0 } -> JSON_INVALID.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_INVALID);
	putchar('\n');
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_INVALID",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_INVALID);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_FALSE",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_FALSE);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_TRUE",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_TRUE);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_BOOLEAN",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_BOOLEAN);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_NULL",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_NULL);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_NUMBER",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_NUMBER);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_STRING",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_STRING);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_ARRAY",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_ARRAY);
	printf("json_flag::%-13s  0x%02"
		"x"
		"\n",
		"JSON_OBJECT",
		(struct {
		uint8_t JSON_INVALID, JSON_FALSE, JSON_TRUE,
			JSON_BOOLEAN, JSON_NULL, JSON_NUMBER,
			JSON_STRING, JSON_ARRAY, JSON_OBJECT;
	})
	{
		(_Generic((json_flag*) { 0 } -> JSON_INVALID.b0,
			typeof((*(null_type*)0).b0) : (uint8_t)0,
			typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b1,
				typeof((*(null_type*)0).b1) : (uint8_t)0,
				typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b2,
				typeof((*(null_type*)0).b2) : (uint8_t)0,
				typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b3,
				typeof((*(null_type*)0).b3) : (uint8_t)0,
				typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b4,
				typeof((*(null_type*)0).b4) : (uint8_t)0,
				typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b5,
				typeof((*(null_type*)0).b5) : (uint8_t)0,
				typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b6,
				typeof((*(null_type*)0).b6) : (uint8_t)0,
				typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
			| _Generic((json_flag*) { 0 } -> JSON_INVALID.b7,
				typeof((*(null_type*)0).b7) : (uint8_t)0,
				typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_FALSE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_FALSE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_TRUE.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_TRUE.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_BOOLEAN.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NULL.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NULL.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_NUMBER.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_NUMBER.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_STRING.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_STRING.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_ARRAY.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_ARRAY.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7))),
			(_Generic((json_flag*) { 0 } -> JSON_OBJECT.b0,
				typeof((*(null_type*)0).b0) : (uint8_t)0,
				typeof((*(full_type*)0).b0) : ((uint8_t)1 << 0))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b1,
					typeof((*(null_type*)0).b1) : (uint8_t)0,
					typeof((*(full_type*)0).b1) : ((uint8_t)1 << 1))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b2,
					typeof((*(null_type*)0).b2) : (uint8_t)0,
					typeof((*(full_type*)0).b2) : ((uint8_t)1 << 2))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b3,
					typeof((*(null_type*)0).b3) : (uint8_t)0,
					typeof((*(full_type*)0).b3) : ((uint8_t)1 << 3))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b4,
					typeof((*(null_type*)0).b4) : (uint8_t)0,
					typeof((*(full_type*)0).b4) : ((uint8_t)1 << 4))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b5,
					typeof((*(null_type*)0).b5) : (uint8_t)0,
					typeof((*(full_type*)0).b5) : ((uint8_t)1 << 5))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b6,
					typeof((*(null_type*)0).b6) : (uint8_t)0,
					typeof((*(full_type*)0).b6) : ((uint8_t)1 << 6))
				| _Generic((json_flag*) { 0 } -> JSON_OBJECT.b7,
					typeof((*(null_type*)0).b7) : (uint8_t)0,
					typeof((*(full_type*)0).b7) : ((uint8_t)1 << 7)))
	}
	.JSON_OBJECT);
}
#pragma GCC diagnostic pop
