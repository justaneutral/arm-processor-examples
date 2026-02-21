#ifdef WIN32
uint16_t staticRefInCase21[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefInCase21[] =
#endif
     {
        55587, 30270, 1104, 25120, 14784,
        9571, 12433, 7070, 56975, 9759,
        19872, 47564, 23069, 20121, 39573,
        43953, 7440, 7160, 11309, 23772,
        11220, 2517, 22389, 7578, 54497,

     };

#ifdef WIN32
uint16_t staticRefOutCase21[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefOutCase21[] =
#endif
     {
        55587, 56975, 56975,
        47564, 56975, 56975,
        47564, 47564, 54497,

     };
