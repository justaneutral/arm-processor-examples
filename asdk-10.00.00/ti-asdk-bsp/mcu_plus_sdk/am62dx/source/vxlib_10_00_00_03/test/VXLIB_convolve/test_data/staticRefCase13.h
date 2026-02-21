#ifdef WIN32
uint8_t staticRefInCase13[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase13[] =
#endif
     {
        53, 126, 22, 64, 215,
        0, 247, 64, 143, 20,
        246, 144, 113, 188, 55,
        15, 9, 19, 75, 212,
        114, 79, 161, 66, 82,
        252, 213, 102, 120, 208,
        110, 67, 121, 177, 3,
        67, 193, 40, 73, 214,
        45, 14, 143, 15, 87,

     };

#ifdef WIN32
int16_t staticRefFilterCase13[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase13[] =
#endif
     {
        2, 1, 2,
        2, 0, 0,
        0, 0, 1,

     };

#ifdef WIN32
int16_t staticRefOutCase13[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase13[] =
#endif
     {
        65, 74, 36,
        18, 50, 44,
        57, 46, 68,
        78, 56, 57,
        52, 58, 62,
        56, 71, 43,
        36, 25, 64,

     };
