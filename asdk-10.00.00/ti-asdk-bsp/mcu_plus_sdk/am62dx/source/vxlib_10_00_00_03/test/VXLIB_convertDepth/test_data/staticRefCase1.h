#ifdef WIN32
uint8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn0Case1[] =
#endif
     {
        112, 229, 46,
        127, 139, 167,
        133, 51, 145,

     };

#ifdef WIN32
int16_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase1[] =
#endif
     {
        448, 916, 184,
        508, 556, 668,
        532, 204, 580,

     };
