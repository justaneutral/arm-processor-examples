#ifdef WIN32
int8_t staticRefIn0Case8[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn0Case8[] =
#endif
     {
        -44, -91, 108,
        80, -28, 20,
        -112, -74, 6,

     };

#ifdef WIN32
int8_t staticRefIn1Case8[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn1Case8[] =
#endif
     {
        -15, -8, -116,
        -56, 57, -124,
        27, 29, 27,

     };

#ifdef WIN32
uint8_t staticRefOutCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase8[] =
#endif
     {
        59, 99, 224,
        136, 85, 144,
        139, 103, 33,

     };
