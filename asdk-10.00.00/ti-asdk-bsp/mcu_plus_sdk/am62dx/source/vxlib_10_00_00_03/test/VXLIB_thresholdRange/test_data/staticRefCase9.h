#ifdef WIN32
int8_t staticRefInpCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefInpCase9[] =
#endif
     {
        57, 94, -117, 65,
        -28, -125, -72, 41,
        -95, -57, 97, -51,
        89, -38, 107, 35,

     };

#ifdef WIN32
int8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefOutCase9[] =
#endif
     {
        40, 24, 24, 40,
        24, 24, 24, 40,
        24, 24, 24, 24,
        40, 24, 24, 24,

     };
