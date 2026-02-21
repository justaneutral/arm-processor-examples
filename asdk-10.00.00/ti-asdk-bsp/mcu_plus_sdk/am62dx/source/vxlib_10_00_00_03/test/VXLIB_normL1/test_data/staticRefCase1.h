#ifdef WIN32
int8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn0Case1[] =
#endif
     {
        -43, -11, 82,
        100, -109, -11,
        -75, -81, -48,

     };

#ifdef WIN32
int8_t staticRefIn1Case1[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn1Case1[] =
#endif
     {
        41, -17, -93,
        -21, 83, 21,
        1, 75, -10,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        84, 28, 175,
        121, 192, 32,
        76, 156, 58,

     };
