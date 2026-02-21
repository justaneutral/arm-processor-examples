#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        122, 21, 160,
        84, 225, 130,
        217, 59, 16,

     };

#ifdef WIN32
uint8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase9[] =
#endif
     {
        125,

     };
