#ifdef WIN32
uint8_t staticRefInCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase10[] =
#endif
     {
        234, 52, 165,
        112, 105, 184,
        82, 161, 187,

     };

#ifdef WIN32
uint8_t staticRefOutCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase10[] =
#endif
     {
        142,

     };
