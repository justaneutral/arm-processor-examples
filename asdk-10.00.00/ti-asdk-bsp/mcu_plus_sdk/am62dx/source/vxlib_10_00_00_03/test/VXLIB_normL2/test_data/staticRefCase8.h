#ifdef WIN32
int8_t staticRefIn0Case8[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn0Case8[] =
#endif
     {
        96, 57, 8,
        -47, 34, -63,
        124, -29, -78,

     };

#ifdef WIN32
int8_t staticRefIn1Case8[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn1Case8[] =
#endif
     {
        -116, -90, 51,
        79, -104, -50,
        -123, 115, -95,

     };

#ifdef WIN32
uint8_t staticRefOutCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase8[] =
#endif
     {
        151, 107, 52,
        92, 109, 80,
        175, 119, 123,

     };
