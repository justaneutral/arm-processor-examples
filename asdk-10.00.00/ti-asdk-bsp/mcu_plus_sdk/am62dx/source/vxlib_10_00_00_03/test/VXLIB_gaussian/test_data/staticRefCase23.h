#ifdef WIN32
uint8_t staticRefInCase23[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase23[] =
#endif
     {
        47, 120, 123, 11, 127,
        62, 47, 218, 44, 122,
        148, 25, 79, 0, 23,
        19, 169, 221, 231, 125,
        110, 147, 195, 175, 192,

     };

#ifdef WIN32
uint8_t staticRefOutCase23[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase23[] =
#endif
     {
        110,

     };
