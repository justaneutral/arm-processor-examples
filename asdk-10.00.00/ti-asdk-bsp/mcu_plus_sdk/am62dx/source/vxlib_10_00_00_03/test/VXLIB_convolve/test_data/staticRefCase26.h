#ifdef WIN32
uint8_t staticRefInCase26[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase26[] =
#endif
     {
        61, 117, 131, 249, 154, 89,
        38, 180, 69, 89, 155, 156,
        128, 210, 103, 236, 40, 13,
        120, 122, 176, 205, 88, 156,
        177, 224, 55, 133, 87, 219,
        208, 68, 70, 81, 245, 101,

     };

#ifdef WIN32
int16_t staticRefFilterCase26[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase26[] =
#endif
     {
        2, 1, 0, -1, 2,
        2, -1, -1, 2, 2,
        1, 1, 0, 0, 0,
        0, 0, 1, 1, 1,
        0, 1, 0, 1, 1,

     };

#ifdef WIN32
int16_t staticRefOutCase26[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase26[] =
#endif
     {
        106, 145,
        171, 155,

     };
