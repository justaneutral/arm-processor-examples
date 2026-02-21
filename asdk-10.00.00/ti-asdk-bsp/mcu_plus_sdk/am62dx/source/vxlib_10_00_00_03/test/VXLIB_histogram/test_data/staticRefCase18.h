#ifdef WIN32
uint8_t staticRefInCase18[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase18[] =
#endif
     {
        87, 116, 163, 171,
        214, 191, 128, 23,
        10, 91, 226, 95,
        194, 175, 158, 206,

     };

#ifdef WIN32
uint32_t staticRefOutCase18[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefOutCase18[] =
#endif
     {
        1, 1, 0, 0, 0, 3, 0, 2, 0, 1, 3, 1, 2, 1, 1, 0,
     };
