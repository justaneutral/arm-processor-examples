#ifdef WIN32
int8_t staticRefInCase61[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefInCase61[] =
#endif
     {
        -108, -65, 10, -58,
        -51, 27, -36, 33,

     };

#ifdef WIN32
int8_t staticRefOutCase61[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefOutCase61[] =
#endif
     {
        107, 64, -11, 57,
        50, -28, 35, -34,

     };
