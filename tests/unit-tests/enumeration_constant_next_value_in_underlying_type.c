/* after an initializer, the next value is computed in the fixed type */
enum E : unsigned char {
    A = 254,
    B,
    C
}; //lint 860 enumerator overflow
