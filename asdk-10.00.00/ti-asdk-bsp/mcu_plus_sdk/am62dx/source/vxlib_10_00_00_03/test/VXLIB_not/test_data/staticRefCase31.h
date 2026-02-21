#ifdef WIN32
uint16_t staticRefInCase31[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefInCase31[] =
#endif
     {
        55369, 38288, 26225,
        34848, 63990, 37946,
        36288, 7386, 26061,

     };

#ifdef WIN32
uint16_t staticRefOutCase31[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefOutCase31[] =
#endif
     {
        10166, 27247, 39310,
        30687, 1545, 27589,
        29247, 58149, 39474,

     };
