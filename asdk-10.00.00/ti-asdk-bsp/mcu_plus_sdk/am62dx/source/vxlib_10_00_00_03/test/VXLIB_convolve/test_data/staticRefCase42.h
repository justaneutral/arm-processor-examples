#ifdef WIN32
uint8_t staticRefInCase42[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase42[] =
#endif
     {
        19, 195, 205, 166, 189, 152, 190, 130, 48,
        9, 90, 246, 181, 154, 186, 51, 204, 133,
        57, 69, 203, 86, 150, 211, 198, 160, 87,
        165, 244, 138, 153, 66, 17, 162, 199, 28,
        198, 115, 53, 223, 37, 18, 4, 210, 137,
        158, 11, 169, 46, 184, 184, 0, 99, 250,
        171, 43, 116, 18, 107, 165, 17, 251, 48,
        66, 218, 57, 229, 187, 128, 173, 247, 103,
        185, 34, 191, 2, 45, 111, 143, 47, 131,

     };

#ifdef WIN32
int16_t staticRefFilterCase42[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase42[] =
#endif
     {
        2, 2, 0, 1, 1, 0, -1,
        -1, 1, 0, 0, 2, 0, 0,
        2, 1, 0, 1, 0, 2, 1,
        -1, 1, -1, 0, 1, 0, 1,
        0, 0, 0, 1, 0, 0, -1,
        1, 1, 1, 2, 0, 2, 0,
        0, 0, 1, 0, 0, 2, 1,

     };

#ifdef WIN32
int16_t staticRefOutCase42[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase42[] =
#endif
     {
        99, 116, 126,
        95, 110, 129,
        87, 88, 92,

     };
