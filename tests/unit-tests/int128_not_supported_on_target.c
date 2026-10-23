/* like gcc and msvc, __int128 does not exist on targets without 128-bit integers */
#ifndef __SIZEOF_INT128__
__int128 a; //lint 860 '__int128' is not supported on this target
#endif
