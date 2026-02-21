#ifdef WIN32
uint8_t staticRefInCase5[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase5[] =
#endif
     {
        163, 232, 1, 26, 67,
        233, 151, 28, 248, 136,
        240, 102, 14, 166, 211,
        77, 16, 172, 31, 55,
        6, 49, 97, 38, 84,
        86, 129, 211, 75, 97,
        92, 105, 251, 215, 4,
        214, 157, 2, 21, 65,
        94, 150, 130, 169, 145,

     };

#ifdef WIN32
int16_t staticRefFilterCase5[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase5[] =
#endif
     {
        0, 1, -1,
        1, -1, -1,
        0, 0, 0,

     };

#ifdef WIN32
uint8_t staticRefOutCase5[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase5[] =
#endif
     {
        0, 0, 0,
        0, 12, 0,
        7, 0, 0,
        5, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 5,

     };
