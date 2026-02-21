#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        44, 201, 33, 184, 221, 72,
        171, 154, 209, 99, 129, 213,
        4, 83, 193, 76, 90, 169,
        89, 146, 17, 205, 177, 231,
        67, 212, 95, 100, 241, 120,
        242, 49, 217, 164, 171, 181,

     };

#ifdef WIN32
int16_t staticRefFilterCase9[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase9[] =
#endif
     {
        1, -1, 0,
        1, 1, 0,
        0, 1, -1,

     };

#ifdef WIN32
int16_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase9[] =
#endif
     {
        39, 1, 24, 28,
        8, 32, 1, 21,
        7, 21, 25, 18,
        33, 0, 33, 21,

     };
