#ifdef WIN32
uint8_t staticRefInCase15[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase15[] =
#endif
     {
        111, 218, 215, 137,
        21, 42, 221, 11,

     };

#ifdef WIN32
uint8_t staticRefOutCase15[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase15[] =
#endif
     {
        144, 37, 40, 118,
        234, 213, 34, 244,

     };
