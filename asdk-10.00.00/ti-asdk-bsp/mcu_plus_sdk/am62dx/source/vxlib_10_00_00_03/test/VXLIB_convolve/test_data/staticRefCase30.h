#ifdef WIN32
uint8_t staticRefInCase30[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase30[] =
#endif
     {
        144, 239, 237, 122, 174,
        160, 111, 133, 160, 56,
        0, 10, 230, 89, 25,
        89, 95, 229, 88, 108,
        84, 28, 102, 141, 76,
        12, 95, 197, 185, 181,
        230, 12, 127, 85, 148,
        76, 142, 203, 170, 210,
        76, 243, 125, 163, 129,

     };

#ifdef WIN32
int16_t staticRefFilterCase30[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase30[] =
#endif
     {
        1, 0, 2, 1, 1,
        2, 0, 1, -1, -1,
        2, 1, 1, 0, 2,
        -1, -1, 1, 1, -1,
        2, -1, -1, -1, 0,

     };

#ifdef WIN32
int16_t staticRefOutCase30[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase30[] =
#endif
     {
        40,
        85,
        88,
        88,
        116,

     };
