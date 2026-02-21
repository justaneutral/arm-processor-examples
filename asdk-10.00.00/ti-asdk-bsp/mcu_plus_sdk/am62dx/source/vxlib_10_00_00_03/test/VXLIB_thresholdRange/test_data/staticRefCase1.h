#ifdef WIN32
uint8_t staticRefInpCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInpCase1[] =
#endif
     {
        113, 172, 96, 106,
        65, 157, 123, 25,
        114, 205, 15, 25,
        140, 102, 133, 186,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,

     };
