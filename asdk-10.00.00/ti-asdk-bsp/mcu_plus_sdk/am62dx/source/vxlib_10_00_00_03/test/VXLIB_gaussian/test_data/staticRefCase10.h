#ifdef WIN32
uint8_t staticRefInCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase10[] =
#endif
     {
        207, 85, 27,
        26, 112, 6,
        49, 219, 173,

     };

#ifdef WIN32
uint8_t staticRefOutCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase10[] =
#endif
     {
        98,

     };
