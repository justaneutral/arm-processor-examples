#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        145, 131, 2, 87, 22, 254, 206, 0,
        28, 149, 182, 163, 215, 76, 60, 170,

     };

#ifdef WIN32
uint32_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefOutCase1[] =
#endif
     {
        1, 2, 0, 1, 1, 1, 0, 0, 1, 2, 2, 1, 1, 1, 0, 1,
     };
