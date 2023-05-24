# Removing Phase 2

## Abstract

This proposal aims to remove phase 2 from the C standard by creating grammar rules that describe the specific positions
where line continuation (backslash + new-line) can occurs following the convention already used in real programs.

The current description of phase 2 allows line continuation to be used at ANY position, and it is important to emphasize this fact.

The objective of this proposal is to gather usage patterns of line continuation in the field and translate them into precise grammar rules.

## Current Usage Research

### 1 - Preprocessor
The most common usage of line continuation is within preprocessor directives, where new-line has a syntax significance.
Programmers use line continuation to write multi-line preprocessor directives.

```c
#define M(a)   (a && \
               1 )
```

### 2 - Line Comments

```c
 // line \
    comment
```

### 3 - Comments

```c
  /*								\
	 * Because of FreeBSD, we care about gcc 4.2, which doesn't have\
	 * an atomic exchange builtin.  We fake it with a CAS loop.	\
	 */								\

#define M \
	/*								\
	*/								\
  1\
}		


const char* paymentrequest1_cert1_BASE64 =
"\
Egt4NTA5K3NoYTI1NhrxAwruAzCCAeowggFToAMCAQICAQEwDQYJKoZIhvcNAQEL\
BQAwITEfMB0GA1UEAxMWUGF5bWVudFJlcXVlc3QgVGVzdCBDQTAeFw0xMjEyMTAx\
NjM3MjRaFw0yMjEyMDgxNjM3MjRaMEMxGTAXBgNVBAMMEHRlc3RtZXJjaGFudC5v\
cmcxJjAkBgNVBAoMHVBheW1lbnQgUmVxdWVzdCBUZXN0IE1lcmNoYW50MIGfMA0G\
CSqGSIb3DQEBAQUAA4GNADCBiQKBgQDHkMy8W1u6HsWlSqdWTmMKf54gICxNfxbY\
+rcMtAftr62hCYx2d2QiSRd1pCUzmo12IiSX3WxSHwaTnT3MFD6jRx6+zM6XdGar\
I2zpYle11ANzu4gAthN17uRQHV2O5QxVtzNaMdKeJLXT2L9tfEdyL++9ZUqoQmdA\
YG9ix330hQIDAQABoxAwDjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4GB\
AIkyO99KC68bi9PFRyQQ7nvn5GlQEb3Ca1bRG5+AKN9N5vc8rZ9G2hejtM8wEXni\
eGBP+chVMsbTPEHKLrwREn7IvcyCcbAStaklPC3w0B/2idQSHskb6P3X13OR2bTH\
a2+6wuhsOZRUrVNr24rM95DKx/eCC6JN1VW+qRPU6fqzIjQSHwiw2wYSGXapFJVg\
igPI+6XpExtNLO/i1WFV8ZmoiKwYsuHFiwUqC1VuaXRUZXN0T25lKoABS0j59iMU\
Uc9MdIfwsO1BskIET0eJSGNZ7eXb9N62u+qf831PMpEHkmlGpk8rHy92nPcgua/U\
Yt8oZMn3QaTZ5A6HjJbc3A73eLylp1a0SwCl+KDMEvDQhqMn1jAVu2v92AH3uB7n\
SiWVbw0tX/68iSQEGGfh9n6ee/8Myb3ICdw=\
";

```

### 4 - Literal Strings

```c
   s = "\
Even if you've mastered the art of the cover letter and the resume, \
another part of the job search process can trip up an otherwise \
qualified candidate: the writing sample.\n\
\n";

```

Based on these cases, we can incorporate the following rules into the grammar:

- Backslash + new-line can occur inside literal strings, line comments, and comments.
- For preprocessor directives, line continuation is considered a token that is blank.
- C++ 23 allow spaces after backslash and before newline. This token that is blank only in
  preprocessor directives can be defined  in the same way.
  
## Breaking Changes

```c
#define A B\
C
```

Instead of generating "BC," it will generate "B C." 

I conducted a search and found 0 cases of this usage. 
[Here is the search result](https://sourcegraph.com/search?q=context:global+%5Cw%5C%5C%5Cr%3F%5Cn%5Cw+lang:C+lang:C%2B%2B+count:2000&patternType=regexp&sm=0&groupBy=repo).

To prevent potential issues with this case, we can define that line continuation inside identifier tokens
is an error. 

So far all changes affect only tokens.


