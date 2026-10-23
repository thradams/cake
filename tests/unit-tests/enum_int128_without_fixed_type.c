/* like gcc and clang, without fixed type an enum is never wider than 64 bits */
#ifdef __SIZEOF_INT128__

enum E {
    A = (unsigned __int128) 1 << 64
}; //lint 860 enum value range unsupported

#endif
