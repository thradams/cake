
    constexpr short q3[4][3][2] = { {
                                       {1},
                                   },
                                   {
                                       {2, 3},
                                   },
                                   {
                                       {4, 5},
                                       {6},
                                   } };


    static_assert(q3[0][0][0] == 1);

    static_assert(q3[1][0][0] == 2);
    static_assert(q3[1][0][1] == 3);

    static_assert(q3[2][0][0] == 4);
    static_assert(q3[2][0][1] == 5);
    static_assert(q3[2][1][0] == 6);