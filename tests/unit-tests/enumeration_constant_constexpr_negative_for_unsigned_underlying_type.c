/* the value comes from a constexpr object: the sign is read from the referenced object */
constexpr int n = -1;
enum E : unsigned int {
    A = n
}; //lint 860 enumerator value outside of underlying type range
