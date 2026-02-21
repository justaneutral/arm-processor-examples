#ifdef WIN32
uint8_t staticRefInCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase8[] =
#endif
     {
        24, 151, 164,
        60, 98, 43,
        36, 95, 219,

     };

#ifdef WIN32
uint8_t staticRefOutCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase8[] =
#endif
     {
        231, 104, 91,
        195, 157, 212,
        219, 160, 36,

     };
