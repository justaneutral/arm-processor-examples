#ifdef WIN32
uint8_t staticRefInCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase22[] =
#endif
     {
        221, 244, 41, 57, 17,
        51, 76, 130, 135, 217,
        115, 9, 33, 41, 135,
        39, 225, 54, 190, 16,
        112, 192, 88, 36, 74,

     };

#ifdef WIN32
uint8_t staticRefOutCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase22[] =
#endif
     {
        89,

     };
