#ifdef WIN32
uint8_t staticRefInCase17[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase17[] =
#endif
     {
        226, 226, 81, 122, 162, 65, 142,
        127, 200, 215, 155, 37, 148, 162,
        233, 216, 227, 13, 91, 127, 194,
        160, 167, 64, 120, 111, 213, 237,
        153, 2, 98, 190, 161, 199, 139,
        229, 77, 98, 149, 153, 67, 144,
        24, 106, 232, 209, 94, 191, 225,

     };

#ifdef WIN32
int16_t staticRefFilterCase17[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase17[] =
#endif
     {
        1, -1, 0, 1, 0,
        0, 2, 0, 0, -1,
        -1, 0, 0, 0, 0,
        1, 0, 1, 1, 2,
        0, 1, 1, 2, 1,

     };

#ifdef WIN32
uint8_t staticRefOutCase17[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase17[] =
#endif
     {
        96, 103, 102,
        130, 108, 109,
        91, 116, 75,

     };
