enum constant_value_type {
    TYPE_NOT_CONSTANT,
    TYPE_EMPTY,
    TYPE_LONG_LONG,
    TYPE_DOUBLE,
    TYPE_UNSIGNED_LONG_LONG
};

struct constant_value {
    enum constant_value_type type;
    union {
        unsigned long long ullvalue;
        long long llvalue;
        double dvalue;
    };
};

struct constant_value make_constant_value_double(double d)
{
    struct constant_value r;
    r.dvalue = d;
    r.type = TYPE_DOUBLE;
    return r;
}


int main()
{
    struct constant_value r;
    r.type = TYPE_LONG_LONG;
}