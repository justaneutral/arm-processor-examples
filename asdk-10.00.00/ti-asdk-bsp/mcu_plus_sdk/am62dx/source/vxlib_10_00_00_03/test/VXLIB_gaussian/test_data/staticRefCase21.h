#ifdef WIN32
uint8_t staticRefInCase21[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase21[] =
#endif
     {
        82, 157, 116, 3, 22,
        85, 88, 95, 128, 241,
        31, 24, 97, 109, 112,
        240, 218, 13, 193, 81,
        227, 246, 76, 167, 67,

     };

#ifdef WIN32
uint8_t staticRefOutCase21[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase21[] =
#endif
     {
        159,

     };
