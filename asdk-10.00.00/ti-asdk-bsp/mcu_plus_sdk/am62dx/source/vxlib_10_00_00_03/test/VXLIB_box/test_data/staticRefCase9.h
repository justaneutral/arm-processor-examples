#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        183, 171, 166,
        251, 29, 76,
        188, 115, 163,

     };

#ifdef WIN32
uint8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase9[] =
#endif
     {
        149,

     };
