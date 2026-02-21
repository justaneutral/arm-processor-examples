#ifdef WIN32
uint8_t staticRefInpCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInpCase1[] =
#endif
     {
        62, 170, 251, 110,
        186, 25, 79, 188,
        204, 237, 236, 29,
        20, 106, 41, 33,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,

     };
