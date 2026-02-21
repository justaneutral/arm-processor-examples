#ifdef WIN32
uint8_t staticRefInCase11[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase11[] =
#endif
     {
        215, 51, 2, 151, 247,
        9, 48, 89, 141, 205,
        58, 82, 244, 121, 222,
        234, 121, 245, 228, 197,
        34, 12, 253, 45, 250,

     };

#ifdef WIN32
uint8_t staticRefOutCase11[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase11[] =
#endif
     {
        215, 151, 247,
        244, 244, 244,
        245, 245, 245,
        253, 253, 253,

     };
