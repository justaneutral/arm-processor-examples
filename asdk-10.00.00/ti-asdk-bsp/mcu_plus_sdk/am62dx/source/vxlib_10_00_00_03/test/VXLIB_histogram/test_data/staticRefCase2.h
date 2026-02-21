#ifdef WIN32
uint8_t staticRefInCase2[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase2[] =
#endif
     {
        111, 58, 205, 142, 196, 216, 121, 141, 56, 89, 247, 169, 20, 170, 75, 114,
        209, 103, 43, 66, 28, 202, 183, 113, 60, 186, 82, 21, 114, 198, 15, 36,

     };

#ifdef WIN32
uint32_t staticRefOutCase2[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefOutCase2[] =
#endif
     {
        1, 3, 2, 3, 2, 2, 3, 3, 2, 0, 2, 2, 4, 2, 0, 1,
     };
