#ifdef WIN32
int8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn0Case1[] =
#endif
     {
        48, -94, 63,
        29, -101, 4,
        -20, -8, -33,

     };

#ifdef WIN32
int8_t staticRefIn1Case1[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn1Case1[] =
#endif
     {
        -98, -101, -100,
        -92, -104, 4,
        104, 36, 53,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        109, 138, 118,
        96, 145, 6,
        106, 37, 62,

     };
