#ifdef WIN32
uint8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn0Case1[] =
#endif
     {
        69, 65, 202,
        189, 78, 117,
        83, 82, 121,

     };

#ifdef WIN32
uint8_t staticRefIn1Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn1Case1[] =
#endif
     {
        235, 79, 122,
        65, 7, 18,
        52, 6, 179,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        65, 65, 74,
        1, 6, 16,
        16, 2, 49,

     };
