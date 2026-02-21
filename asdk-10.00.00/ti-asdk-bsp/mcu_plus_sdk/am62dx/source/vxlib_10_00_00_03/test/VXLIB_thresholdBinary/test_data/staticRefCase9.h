#ifdef WIN32
int8_t staticRefInpCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefInpCase9[] =
#endif
     {
        100, -83, -45, 4,
        -5, -91, -15, 85,
        78, -85, 118, 74,
        -69, -114, -104, 12,

     };

#ifdef WIN32
int8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefOutCase9[] =
#endif
     {
        1, 0, 0, 0,
        0, 0, 0, 1,
        1, 0, 1, 1,
        0, 0, 0, 1,

     };
