#ifdef WIN32
uint16_t staticRefInCase31[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefInCase31[] =
#endif
     {
        1926, 60058, 33185, 44636, 33926,
        47525, 40954, 42353, 15067, 48227,
        48367, 26998, 17829, 58187, 8203,
        14188, 3711, 10268, 45935, 50333,
        44546, 11697, 29008, 52425, 20151,

     };

#ifdef WIN32
uint16_t staticRefOutCase31[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefOutCase31[] =
#endif
     {
        60058, 60058, 48227,
        48367, 58187, 58187,
        48367, 58187, 58187,
        44546, 52425, 52425,

     };
