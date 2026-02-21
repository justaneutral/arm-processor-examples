#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        169, 245, 84, 91, 181,
        129, 201, 29, 83, 101,
        211, 122, 129, 46, 74,
        22, 16, 57, 211, 32,
        137, 91, 203, 100, 71,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        245, 245, 181,
        211, 211, 211,
        211, 211, 211,

     };
