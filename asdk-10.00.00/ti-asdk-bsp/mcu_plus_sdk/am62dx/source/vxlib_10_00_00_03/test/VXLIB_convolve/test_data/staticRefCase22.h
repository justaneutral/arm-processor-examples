#ifdef WIN32
uint8_t staticRefInCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase22[] =
#endif
     {
        116, 177, 77, 107, 226,
        160, 152, 56, 130, 242,
        210, 187, 247, 4, 0,
        253, 160, 134, 241, 14,
        37, 240, 125, 45, 66,
        179, 23, 228, 100, 123,
        190, 96, 17, 172, 173,
        182, 12, 211, 80, 169,
        37, 254, 56, 195, 152,

     };

#ifdef WIN32
int16_t staticRefFilterCase22[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase22[] =
#endif
     {
        1, 0, 0, -1, 1,
        -1, 1, 2, 1, 1,
        0, 1, 2, 0, 1,
        0, -1, 1, 0, -1,
        1, 0, 2, 2, 1,

     };

#ifdef WIN32
uint8_t staticRefOutCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase22[] =
#endif
     {
        130,
        149,
        122,
        143,
        102,

     };
