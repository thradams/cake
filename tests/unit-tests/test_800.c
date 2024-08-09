 const char* s = "part1" "part2";

#define S "part1" "part2"
static_assert(sizeof(S)==11);

