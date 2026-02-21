#ifdef WIN32
uint8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn0Case1[] =
#endif
     {
        101, 22, 36,
        7, 39, 102,
        69, 50, 8,

     };

#ifdef WIN32
uint8_t staticRefIn1Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn1Case1[] =
#endif
     {
        255, 255, 0,
        0, 255, 255,
        255, 255, 0,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        39,

     };
