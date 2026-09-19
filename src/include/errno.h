/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#define __STDC_VERSION_ERRNO_H__ 202311L

#if defined(_WIN32)
int* _errno(void);
#define errno (*_errno())
#elif defined(__APPLE__)
int* __error(void);
#define errno (*__error())
#elif defined(__linux__)
int* __errno_location(void);
#define errno (*__errno_location())
#else
extern int errno;
#endif

/* values shared by windows, linux and macOS */
#define EPERM           1
#define ENOENT          2
#define ESRCH           3
#define EINTR           4
#define EIO             5
#define ENXIO           6
#define E2BIG           7
#define ENOEXEC         8
#define EBADF           9
#define ECHILD          10
#define ENOMEM          12
#define EACCES          13
#define EFAULT          14
#define EBUSY           16
#define EEXIST          17
#define EXDEV           18
#define ENODEV          19
#define ENOTDIR         20
#define EISDIR          21
#define EINVAL          22
#define ENFILE          23
#define EMFILE          24
#define ENOTTY          25
#define EFBIG           27
#define ENOSPC          28
#define ESPIPE          29
#define EROFS           30
#define EMLINK          31
#define EPIPE           32
#define EDOM            33
#define ERANGE          34

#if defined(_WIN32)

#define EAGAIN          11
#define EDEADLK         36
#define ENAMETOOLONG    38
#define ENOLCK          39
#define ENOSYS          40
#define ENOTEMPTY       41
#define EILSEQ          42
#define EADDRINUSE      100
#define EADDRNOTAVAIL   101
#define EAFNOSUPPORT    102
#define EALREADY        103
#define EBADMSG         104
#define ECANCELED       105
#define ECONNABORTED    106
#define ECONNREFUSED    107
#define ECONNRESET      108
#define EDESTADDRREQ    109
#define EHOSTUNREACH    110
#define EIDRM           111
#define EINPROGRESS     112
#define EISCONN         113
#define ELOOP           114
#define EMSGSIZE        115
#define ENETDOWN        116
#define ENETRESET       117
#define ENETUNREACH     118
#define ENOBUFS         119
#define ENODATA         120
#define ENOLINK         121
#define ENOMSG          122
#define ENOPROTOOPT     123
#define ENOSR           124
#define ENOSTR          125
#define ENOTCONN        126
#define ENOTRECOVERABLE 127
#define ENOTSOCK        128
#define ENOTSUP         129
#define EOPNOTSUPP      130
#define EOVERFLOW       132
#define EOWNERDEAD      133
#define EPROTO          134
#define EPROTONOSUPPORT 135
#define EPROTOTYPE      136
#define ETIME           137
#define ETIMEDOUT       138
#define ETXTBSY         139
#define EWOULDBLOCK     140

#elif defined(__APPLE__)

#define EAGAIN          35
#define EDEADLK         11
#define ENAMETOOLONG    63
#define ENOLCK          77
#define ENOSYS          78
#define ENOTEMPTY       66
#define EILSEQ          92
#define EADDRINUSE      48
#define EADDRNOTAVAIL   49
#define EAFNOSUPPORT    47
#define EALREADY        37
#define EBADMSG         94
#define ECANCELED       89
#define ECONNABORTED    53
#define ECONNREFUSED    61
#define ECONNRESET      54
#define EDESTADDRREQ    39
#define EHOSTUNREACH    65
#define EIDRM           90
#define EINPROGRESS     36
#define EISCONN         56
#define ELOOP           62
#define EMSGSIZE        40
#define ENETDOWN        50
#define ENETRESET       52
#define ENETUNREACH     51
#define ENOBUFS         55
#define ENODATA         96
#define ENOLINK         97
#define ENOMSG          91
#define ENOPROTOOPT     42
#define ENOSR           98
#define ENOSTR          99
#define ENOTCONN        57
#define ENOTRECOVERABLE 104
#define ENOTSOCK        38
#define ENOTSUP         45
#define EOPNOTSUPP      102
#define EOVERFLOW       84
#define EOWNERDEAD      105
#define EPROTO          100
#define EPROTONOSUPPORT 43
#define EPROTOTYPE      41
#define ETIME           101
#define ETIMEDOUT       60
#define ETXTBSY         26
#define EWOULDBLOCK     EAGAIN

#else /* linux and others */

#define EAGAIN          11
#define EDEADLK         35
#define ENAMETOOLONG    36
#define ENOLCK          37
#define ENOSYS          38
#define ENOTEMPTY       39
#define ELOOP           40
#define EILSEQ          84
#define EADDRINUSE      98
#define EADDRNOTAVAIL   99
#define EAFNOSUPPORT    97
#define EALREADY        114
#define EBADMSG         74
#define ECANCELED       125
#define ECONNABORTED    103
#define ECONNREFUSED    111
#define ECONNRESET      104
#define EDESTADDRREQ    89
#define EHOSTUNREACH    113
#define EIDRM           43
#define EINPROGRESS     115
#define EISCONN         106
#define EMSGSIZE        90
#define ENETDOWN        100
#define ENETRESET       102
#define ENETUNREACH     101
#define ENOBUFS         105
#define ENODATA         61
#define ENOLINK         67
#define ENOMSG          42
#define ENOPROTOOPT     92
#define ENOSR           63
#define ENOSTR          60
#define ENOTCONN        107
#define ENOTRECOVERABLE 131
#define ENOTSOCK        88
#define ENOTSUP         95
#define EOPNOTSUPP      95
#define EOVERFLOW       75
#define EOWNERDEAD      130
#define EPROTO          71
#define EPROTONOSUPPORT 93
#define EPROTOTYPE      91
#define ETIME           62
#define ETIMEDOUT       110
#define ETXTBSY         26
#define EWOULDBLOCK     EAGAIN

#endif

#else
#include_next <errno.h>
#endif
