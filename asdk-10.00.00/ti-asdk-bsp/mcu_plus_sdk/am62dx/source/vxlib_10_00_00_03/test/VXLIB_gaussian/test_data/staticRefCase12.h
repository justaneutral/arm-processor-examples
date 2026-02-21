#ifdef WIN32
uint8_t staticRefInCase12[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase12[] =
#endif
     {
        80, 92, 104,
        185, 68, 56,
        179, 215, 237,
        66, 212, 44,
        162, 8, 216,
        61, 220, 31,
        132, 18, 19,
        168, 169, 25,
        12, 121, 181,
        20, 82, 34,
        196, 38, 23,
        120, 14, 94,
        231, 14, 153,
        77, 206, 217,
        181, 180, 99,
        231, 81, 136,

     };

#ifdef WIN32
uint8_t staticRefOutCase12[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase12[] =
#endif
     {
        123,
        162,
        144,
        115,
        102,
        89,
        105,
        101,
        72,
        65,
        74,
        110,
        154,
        157,

     };
