#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        88, 44, 167, 61, 172, 53,
        23, 0, 98, 207, 133, 63,
        23, 90, 243, 15, 46, 250,
        18, 109, 3, 42, 118, 18,
        149, 73, 227, 118, 229, 212,
        135, 241, 227, 208, 172, 216,

     };

#ifdef WIN32
int16_t staticRefFilterCase1[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase1[] =
#endif
     {
        0, 0, 1,
        2, 2, 0,
        0, 1, 2,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        27, 59, 82, 43,
        45, 45, 33, 73,
        31, 36, 65, 29,
        55, 72, 60, 80,

     };
