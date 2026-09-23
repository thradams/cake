//6.10.11 Pragma operator

#define LISTING(x) PRAGMA(listing on #x)
#define PRAGMA(x) _Pragma(#x)

LISTING (..listing.dir)
