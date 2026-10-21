/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#if defined(_WIN32) && defined(_M_X64)
    /*
       The msvc x64 va_start calls __va_start(&ap, ...), which only writes
       ap. Declared here, before the msvc header declares it, so the flow
       analysis does not take the call as a read of the still null ap
       (warning 35).
    */
    void __cdecl __va_start(char* _Out* ap, ...);
#endif

#include_next <vadefs.h>
